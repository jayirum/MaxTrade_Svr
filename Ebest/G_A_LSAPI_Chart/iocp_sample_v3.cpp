// iocp_broadcast_refactored.cpp
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <thread>
#include <chrono>
#include <algorithm>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

//====================== 상수/설정 ======================
constexpr size_t CAP_SIZE = 4096;            // SPSC 큐 용량(2의 거듭제곱 권장)
constexpr size_t kMaxLine = 64 * 1024;       // 한 줄 최대 길이(DoS 방지)
constexpr ULONG_PTR kStrandKey = 0xFFFF'FFFF'FFFF'FFFEULL;


//================ for gracefule closing ============================

constexpr ULONG_PTR kShutdownKey = 0xFFFF'FFFF'FFFF'FFFDULL;

std::atomic<bool> g_stopping{ false };
int g_nWorkers = 0;
std::vector<std::thread> g_workers;
std::thread g_ticker;

void request_shutdown() {
    bool expected = false;
    if (!g_stopping.compare_exchange_strong(expected, true)) return; // 이미 진행 중

    // 1) listen 먼저 닫아서 모든 AcceptEx를 실패 완료시킴
    if (g_listen != INVALID_SOCKET) {
        closesocket(g_listen);
        g_listen = INVALID_SOCKET;
    }

    // 2) 현재 열린 모든 세션 소켓 graceful → abortive 순서로 종료 시도
    auto snap = std::atomic_load(&g_sessions);
    if (snap) {
        for (auto& s : *snap) {
            // 선택: 먼저 shutdown으로 FIN 보내보기(원하면)
            shutdown(s->s, SD_BOTH);
            // abortive 로 빨리 끊고 싶으면 기존 헬퍼 사용
            close_clientsock(s->s);
        }
    }

    // 3) IOCP 워커에게 종료 센티넬을 N개 전송
    for (int i = 0; i < g_nWorkers; ++i)
        PostQueuedCompletionStatus(g_iocp, 0, kShutdownKey, nullptr);
}


//====================== 간단 SPSC lock-free 링버퍼 ======================
template <typename T>
class SpscRing {
public:
    bool push(const T& v) {
        auto h = head_.load(std::memory_order_relaxed);
        auto n = next(h);
        if (n == tail_.load(std::memory_order_acquire)) return false; // full
        buf_[h] = v;
        head_.store(n, std::memory_order_release);
        return true;
    }
    bool pop(T& out) {
        auto t = tail_.load(std::memory_order_relaxed);
        if (t == head_.load(std::memory_order_acquire)) return false; // empty
        out = std::move(buf_[t]);
        tail_.store(next(t), std::memory_order_release);
        return true;
    }
    bool empty() const {
        return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
    }
private:
    static_assert((CAP_SIZE& (CAP_SIZE - 1)) == 0, "CAP_SIZE must be power of two");
    static constexpr size_t next(size_t x) { return (x + 1) & (CAP_SIZE - 1); }
    T buf_[CAP_SIZE]{};
    std::atomic<size_t> head_{ 0 }, tail_{ 0 };
};

//====================== 공용 타입 ======================
using ByteVec = std::vector<char>;
using Payload = std::shared_ptr<const ByteVec>;

enum class OvType { Accept, Recv, Send };

//====================== 전방 선언/전역 ======================
struct Session;
using SessionPtr = std::shared_ptr<Session>;

std::atomic<std::shared_ptr<std::vector<SessionPtr>>> g_sessions;
HANDLE  g_iocp = nullptr;
SOCKET  g_listen = INVALID_SOCKET;
LPFN_ACCEPTEX               pAcceptEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS   pGetAddrs = nullptr;

//====================== 유틸 ======================
[[noreturn]] void die(const char* msg) {
    std::fprintf(stderr, "%s (err=%d)\n", msg, (int)WSAGetLastError());
    std::exit(1);
}
void set_nodelay(SOCKET s) {
    BOOL opt = TRUE; setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt));
}
void associate_iocp(HANDLE h, ULONG_PTR key = 0) {
    if (!CreateIoCompletionPort(h, g_iocp, key, 0)) die("CreateIoCompletionPort associate");
}
void ensure_extensions(SOCKET ls) {
    DWORD bytes = 0;
    GUID g1 = WSAID_ACCEPTEX, g2 = WSAID_GETACCEPTEXSOCKADDRS;
    if (WSAIoctl(ls, SIO_GET_EXTENSION_FUNCTION_POINTER, &g1, sizeof(g1),
        &pAcceptEx, sizeof(pAcceptEx), &bytes, NULL, NULL) == SOCKET_ERROR)
        die("WSAIoctl AcceptEx");
    if (WSAIoctl(ls, SIO_GET_EXTENSION_FUNCTION_POINTER, &g2, sizeof(g2),
        &pGetAddrs, sizeof(pGetAddrs), &bytes, NULL, NULL) == SOCKET_ERROR)
        die("WSAIoctl GetAcceptExSockaddrs");
}
void close_clientsock(SOCKET s) {
    linger lg{ TRUE, 0 }; // abortive close
    setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&lg, sizeof(lg));
    closesocket(s);
}

//====================== OV 컨텍스트 ======================
struct OvBase {
    OVERLAPPED ov{};
    OvType type{};
    explicit OvBase(OvType t) : type(t) { ZeroMemory(&ov, sizeof(ov)); }
    virtual ~OvBase() = default;
    static OvBase* From(LPOVERLAPPED p) { return reinterpret_cast<OvBase*>(p); }
};

struct AcceptCtx : OvBase {
    SOCKET sAccept{ INVALID_SOCKET };
    char buf[(sizeof(SOCKADDR_STORAGE) + 16) * 2]{};
    AcceptCtx() : OvBase(OvType::Accept) {}
};

struct RecvCtx : OvBase {
    WSABUF wsa{};
    char buf[16 * 1024]; // 수신 버퍼
    Session* self{};
    RecvCtx() : OvBase(OvType::Recv) {
        wsa.buf = buf; wsa.len = sizeof(buf);
    }
};

struct SendCtx : OvBase {
    SessionPtr selfHold; // 완료까지 세션 생존 보장
    Payload    hold;     // 보낼 데이터(shared_ptr zero-copy)
    explicit SendCtx(SessionPtr s, Payload p)
        : OvBase(OvType::Send), selfHold(std::move(s)), hold(std::move(p)) {
    }
};

struct SendTask : OVERLAPPED {
    SessionPtr self;
    Payload payload;
    SendTask(SessionPtr s, Payload p) : self(std::move(s)), payload(std::move(p)) {
        ZeroMemory(this, sizeof(OVERLAPPED));
    }
};

//====================== 선언만 먼저 ======================
void post_acceptex(int nPosts);
void broadcast(const Payload& p);
void remove_session(const SessionPtr& target);

//====================== 세션 ======================
struct Session : std::enable_shared_from_this<Session> {
    SOCKET s{ INVALID_SOCKET };
    RecvCtx recvCtx{};
    std::string recvAcc;           // TCP 조각 누적 버퍼
    std::atomic<bool> sending{ false };
    SpscRing<Payload> sendQ;

    // enqueue (strand로 funnel)
    void enqueue_send(const Payload& p) {
        auto self = shared_from_this();
        auto* task = new SendTask(self, p);
        PostQueuedCompletionStatus(g_iocp, 0, kStrandKey,
            reinterpret_cast<LPOVERLAPPED>(task));
    }

    // strand(=IOCP 워커 내)에서만 실행
    void handle_send_request(const Payload& p) {
        while (!sendQ.push(p)) _mm_pause();
        kick_send_if_idle();
    }

    void kick_send_if_idle() {
        if (sending.exchange(true, std::memory_order_acq_rel)) return;
        Payload p;
        if (!sendQ.pop(p)) { sending.store(false, std::memory_order_release); return; }
        post_send(std::move(p));
    }

    void post_send(Payload&& p) {
        auto* ctx = new SendCtx(shared_from_this(), std::move(p));
        WSABUF wsa{ (ULONG)ctx->hold->size(), const_cast<char*>(ctx->hold->data()) };
        DWORD sent = 0;
        int rc = WSASend(s, &wsa, 1, &sent, 0, &ctx->ov, nullptr);
        if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
            delete ctx; sending.store(false, std::memory_order_release);
            remove_session(shared_from_this());
            close_clientsock(s);
        }
    }

    // 수신 완료: CRLF로 분리하여 브로드캐스트
    void on_recv_completed(DWORD bytes) {
        if (bytes == 0) return; // 상위에서 종료 처리

        recvAcc.append(recvCtx.buf, recvCtx.buf + bytes);

        // DoS 방지: 라인 상한
        if (recvAcc.size() > kMaxLine) {
            remove_session(shared_from_this());
            close_clientsock(s);
            return;
        }

        size_t pos = 0;
        for (;;) {
            auto eol = recvAcc.find("\r\n", pos);
            if (eol == std::string::npos) break;

            std::string line = recvAcc.substr(pos, eol - pos);
            pos = eol + 2; // CRLF 소비

            // 전송 페이로드 구성: CRLF 포함
            auto msg = std::make_shared<ByteVec>();
            msg->insert(msg->end(), line.begin(), line.end());
            msg->push_back('\r'); msg->push_back('\n');

            broadcast(msg);
        }
        recvAcc.erase(0, pos);

        // 다음 recv 예약
        ZeroMemory(&recvCtx.ov, sizeof(OVERLAPPED));
        DWORD flags = 0, recvd = 0;
        int rc = WSARecv(s, &recvCtx.wsa, 1, &recvd, &flags, &recvCtx.ov, nullptr);
        if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
            remove_session(shared_from_this());
            close_clientsock(s);
        }
    }

    // send 완료 → 다음 예약 or idle
    void on_send_completed(SendCtx* ctx, DWORD /*bytes*/) {
        delete ctx;
        Payload next;
        if (sendQ.pop(next)) {
            post_send(std::move(next));
        }
        else {
            sending.store(false, std::memory_order_release);
            // 경합 보정
            if (sendQ.pop(next) && !sending.exchange(true, std::memory_order_acq_rel))
                post_send(std::move(next));
        }
    }
};

//====================== 세션 목록(COW) 갱신 ======================
void remove_session(const SessionPtr& target) {
    for (;;) {
        auto oldv = std::atomic_load(&g_sessions);
        if (!oldv) return;
        auto nv = std::make_shared<std::vector<SessionPtr>>(*oldv);
        nv->erase(std::remove_if(nv->begin(), nv->end(),
            [&](const SessionPtr& sp) { return sp.get() == target.get(); }),
            nv->end());
        if (std::atomic_compare_exchange_weak(&g_sessions, &oldv, nv)) break;
    }
}

//====================== IOCP 워커 ======================
void iocp_worker() {
    for (;;) {
        DWORD bytes = 0; ULONG_PTR key = 0; LPOVERLAPPED pov = nullptr;
        BOOL ok = GetQueuedCompletionStatus(g_iocp, &bytes, &key, &pov, INFINITE);
                
        // 🔻 종료 센티넬: pov==nullptr, key==kShutdownKey
        if (pov == nullptr && key == kShutdownKey) break;

        if (!ok && pov == nullptr) continue; // timeout or wakeup


        // strand 태스크
        if (pov && key == kStrandKey) {
            auto* task = reinterpret_cast<SendTask*>(pov);
            task->self->handle_send_request(task->payload);
            delete task;
            continue;
        }

        auto* base = OvBase::From(pov);
        switch (base->type) {
        case OvType::Accept: {
            auto* ax = static_cast<AcceptCtx*>(base);
            SOCKET s = ax->sAccept; ax->sAccept = INVALID_SOCKET;

            // AcceptEx 컨텍스트 업데이트 (필수)
            int rc = setsockopt(s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                reinterpret_cast<const char*>(&g_listen), sizeof(g_listen));
            if (rc == SOCKET_ERROR) {
                closesocket(s);
                delete ax;
                
                // 종료 중엔 새 AcceptEx를 다시 걸지 않음
                if (!g_stopping.load(std::memory_order_acquire))
                    post_acceptex(1);
                break;
            }

            set_nodelay(s);
            associate_iocp((HANDLE)s);

            auto sess = std::make_shared<Session>();
            sess->s = s;
            sess->recvCtx.self = sess.get();

            // 세션 목록에 추가(COW)
            for (;;) {
                auto oldv = std::atomic_load(&g_sessions);
                auto nv = std::make_shared<std::vector<SessionPtr>>(oldv ? *oldv : std::vector<SessionPtr>{});
                nv->push_back(sess);
                if (std::atomic_compare_exchange_weak(&g_sessions, &oldv, nv)) break;
            }

            // 첫 수신 예약
            ZeroMemory(&sess->recvCtx.ov, sizeof(OVERLAPPED));
            DWORD flags = 0, recvd = 0;
            rc = WSARecv(s, &sess->recvCtx.wsa, 1, &recvd, &flags, &sess->recvCtx.ov, nullptr);
            if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                remove_session(sess);
                close_clientsock(s);
            }

            delete ax;
            post_acceptex(1); // 계속 깊이 유지
            break;
        }
        case OvType::Recv: {
            auto sess = static_cast<RecvCtx*>(base)->self;
            if (bytes == 0) {
                remove_session(sess->shared_from_this());
                close_clientsock(sess->s);
            }
            else {
                sess->on_recv_completed(bytes);
            }
            break;
        }
        case OvType::Send: {
            auto* ctx = static_cast<SendCtx*>(base);
            ctx->selfHold->on_send_completed(ctx, bytes);
            break;
        }
        }
    }
}

//====================== AcceptEx 보일러 ======================
void post_acceptex(int nPosts) {
    for (int i = 0; i < nPosts; ++i) {
        
        if (g_stopping.load(std::memory_order_acquire)) return;

        auto* ax = new AcceptCtx();
        ax->sAccept = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (ax->sAccept == INVALID_SOCKET) die("WSASocket accept");
        DWORD bytes = 0;
        BOOL ok = pAcceptEx(g_listen, ax->sAccept, ax->buf, 0,
            sizeof(SOCKADDR_STORAGE) + 16,
            sizeof(SOCKADDR_STORAGE) + 16,
            &bytes, &ax->ov);
        if (!ok && WSAGetLastError() != ERROR_IO_PENDING) die("AcceptEx");
    }
}

//====================== 브로드캐스트 ======================
void broadcast(const Payload& p) {
    auto snap = std::atomic_load(&g_sessions);
    if (!snap) return;
    for (auto& s : *snap) {
        s->enqueue_send(p); // 각 세션 strand로 funnel → per-session SPSC 유지
    }
}

//====================== main ======================
int main() {
    WSADATA w; if (WSAStartup(MAKEWORD(2, 2), &w) != 0) die("WSAStartup");
    g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (!g_iocp) die("CreateIoCompletionPort");

    g_listen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (g_listen == INVALID_SOCKET) die("WSASocket listen");

    sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_addr.s_addr = htonl(INADDR_ANY); addr.sin_port = htons(9000);
    if (bind(g_listen, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) die("bind");
    if (listen(g_listen, SOMAXCONN) == SOCKET_ERROR) die("listen");

    associate_iocp((HANDLE)g_listen);
    ensure_extensions(g_listen);

    // IOCP 워커
    const int nWorkers = (std::max)(2u, std::thread::hardware_concurrency());
    std::vector<std::thread> workers;
    workers.reserve(nWorkers);
    for (int i = 0; i < nWorkers; ++i) workers.emplace_back(iocp_worker);

    // AcceptEx 미리 다수 포스트
    post_acceptex(64);

    std::puts("IOCP broadcast server running on port 9000 …");
    std::puts("Clients can send lines; server broadcasts each complete CRLF-terminated line to all clients.");

    // 데모: 1초마다 서버가 "tick\r\n" 브로드캐스트
    std::thread ticker([] {
        for (;;) {
            auto msg = std::make_shared<ByteVec>();
            const char* s = "tick\r\n";
            msg->assign(s, s + 6);
            broadcast(msg);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        });

    ticker.join();
    for (auto& t : workers) t.join();
    WSACleanup();
    return 0;
}
