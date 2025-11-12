// iocp_broadcast_full.cpp
#define WIN32_LEAN_AND_MEAN
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

//====================== 간단 SPSC lock-free 링버퍼 ======================
template <typename T, size_t CAP>
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
    static constexpr size_t next(size_t x) { return (x + 1) % CAP; }
    T buf_[CAP]{};
    std::atomic<size_t> head_{0}, tail_{0};
};

//====================== 공용 타입 ======================
using ByteVec = std::vector<char>;
using Payload = std::shared_ptr<const ByteVec>;

enum class OvType { Accept, Recv, Send };

struct OvBase {
    OVERLAPPED ov{};
    OvType type{};
    explicit OvBase(OvType t) : type(t) { ZeroMemory(&ov, sizeof(ov)); }
    virtual ~OvBase() = default;
    static OvBase* From(LPOVERLAPPED p) { return reinterpret_cast<OvBase*>(p); }
};

struct RecvCtx : OvBase {
    WSABUF wsa{};
    char buf[16 * 1024]; // 수신 버퍼
    Session* self{};
    RecvCtx() : OvBase(OvType::Recv) {
        wsa.buf = buf; wsa.len = sizeof(buf);
    }
};

struct Session;
struct SendCtx : OvBase {
    SessionPtr selfHold;
    //Session* self{};
    Payload hold;
    explicit SendCtx(Session* s, Payload p)
        : OvBase(OvType::Send), selfHold(s), hold(std::move(p)) {}
};

struct AcceptCtx : OvBase {
    SOCKET sAccept{ INVALID_SOCKET };
    char buf[(sizeof(SOCKADDR_STORAGE) + 16) * 2]{};
    AcceptCtx() : OvBase(OvType::Accept) {}
};

//====================== 전방 선언/전역 ======================
struct Session;
using SessionPtr = std::shared_ptr<Session>;

std::atomic<std::shared_ptr<std::vector<SessionPtr>>> g_sessions;
HANDLE  g_iocp = nullptr;
SOCKET  g_listen = INVALID_SOCKET;
LPFN_ACCEPTEX               pAcceptEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS   pGetAddrs = nullptr;

void broadcast(const Payload& p);
void post_acceptex(int nPosts);

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

//====================== 세션 ======================
struct Session : std::enable_shared_from_this<Session> 
{
    SOCKET s{ INVALID_SOCKET };
    RecvCtx recvCtx{};
    std::string recvAcc;                 // TCP 조각 누적 버퍼
    std::atomic<bool> sending{ false };
    SpscRing<Payload, 4096> sendQ;

    // enqueue + 필요 시 kick
    void enqueue_send(const Payload& p) {
        while (!sendQ.push(p)) _mm_pause();
        kick_send_if_idle();
    }

    void kick_send_if_idle() {
        
        if (sending.exchange(true, std::memory_order_acq_rel))
            return; // 이미 sending 값이 true

        Payload p;
        if (!sendQ.pop(p)) { sending.store(false, std::memory_order_release); return; }

        auto* ctx = new SendCtx(this, std::move(p));
        WSABUF wsa{ (ULONG)ctx->hold->size(), const_cast<char*>(ctx->hold->data()) };
        DWORD sent = 0;
        int rc = WSASend(s, &wsa, 1, &sent, 0, &ctx->ov, nullptr);
        if (rc == SOCKET_ERROR) {
            auto e = WSAGetLastError();
            if (e != WSA_IO_PENDING) { closesocket(s); delete ctx; sending.store(false); }
        }
    }

    // 수신 완료 처리: 개행 기준으로 패킷 분리하여 브로드캐스트
    void on_recv_completed(DWORD bytes) {
        if (bytes == 0) { close_clientsock(s); return; }
        recvAcc.append(recvCtx.buf, recvCtx.buf + bytes);

        size_t pos = 0;
        for (;;) {
            auto eol = recvAcc.find('\r\n', pos);
            if (eol == std::string::npos) break;
            std::string line = recvAcc.substr(pos, eol - pos);
            pos = eol + 1;

            // line → payload로 변환(개행 포함해 클라이언트에 보낼 수도)
            auto msg = std::make_shared<ByteVec>();
            msg->assign(line.begin(), line.end());
            msg->push_back('\n');

            broadcast(msg); // 모든 세션에 전파
        }
        recvAcc.erase(0, pos);

        // 다음 recv 예약
        ZeroMemory(&recvCtx.ov, sizeof(OVERLAPPED));
        DWORD flags = 0, recvd = 0;
        int rc = WSARecv(s, &recvCtx.wsa, 1, &recvd, &flags, &recvCtx.ov, nullptr);
        if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) closesocket(s);
    }

    // send 완료 → 다음 send 예약 or 종료
    void on_send_completed(SendCtx* ctx, DWORD /*bytes*/) {
        delete ctx; // 이전 payload 참조 해제

        Payload next;
        if (!sendQ.pop(next))
        {
            sending.store(false, std::memory_order_release);
            // 경합 보정: 막 넣은 게 있으면 재기동
            if (sendQ.pop(next)) 
            {
                if (!sending.exchange(true)) {
                    auto* c2 = new SendCtx(this, std::move(next));
                    WSABUF wsa{ (ULONG)c2->hold->size(), const_cast<char*>(c2->hold->data()) };
                    DWORD sent = 0;
                    int rc = WSASend(s, &wsa, 1, &sent, 0, &c2->ov, nullptr);
                    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) { closesocket(s); delete c2; sending.store(false); }
                }
            }
            return;
        }

        auto* c2 = new SendCtx(this, std::move(next));
        WSABUF wsa{ (ULONG)c2->hold->size(), const_cast<char*>(c2->hold->data()) };
        DWORD sent = 0;
        int rc = WSASend(s, &wsa, 1, &sent, 0, &c2->ov, nullptr);
        if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) { closesocket(s); delete c2; sending.store(false); }
    }
};

void close_clientsock(SOCKET s)
{
    struct linger structLinger;
    structLinger.l_onoff = TRUE;
    structLinger.l_linger = 0;
    setsockopt(s, SOL_SOCKET, SO_LINGER, (LPSTR)&structLinger, sizeof(structLinger));
    closesocket(s);
}

//====================== IOCP 워커 ======================
void remove_session(const SessionPtr& target) {
    // atomic snapshot을 카피-온-라이트로 갱신 (COW: copy-on-write) => 세션목록을 lock 없이 갱신
    for (;;) {
        auto oldv = std::atomic_load(&g_sessions); // // 1) 현재 스냅샷(공유 포인터)을 원자적으로 읽음
        if (!oldv) return;

        auto nv = std::make_shared<std::vector<SessionPtr>>(*oldv); // 2) COW: 깊은 복사로 새 벡터 생성
        nv->erase(
                 std::remove_if(    // 3) target 세션 제거 (erase-remove idiom)
                    nv->begin(), 
                    nv->end(),            
                    [&](const SessionPtr& sp){ return sp.get() == target.get(); }
                    ),
                 nv->end());

        // 4) g_sessions 가 여전히 oldv 를 가리킬 때에만 nv 로 교체 (CAS)
        if (std::atomic_compare_exchange_weak(&g_sessions, &oldv, nv)) 
            break;   //    성공 -> 교체 완료

        //    실패 -> 다른 스레드가 먼저 바꿨음. oldv 에 최신 값이 덮어써졌으므로 다시 루프
    }
}

void iocp_worker() {
    for (;;) {
        DWORD bytes = 0; ULONG_PTR key = 0; LPOVERLAPPED pov = nullptr;
        BOOL ok = GetQueuedCompletionStatus(g_iocp, &bytes, &key, &pov, INFINITE);
        if (!ok && pov == nullptr) continue; // timeout or wakeup

        auto* base = OvBase::From(pov);
        switch (base->type) {
        case OvType::Accept: {
            auto* ax = static_cast<AcceptCtx*>(base);
            SOCKET s = ax->sAccept; ax->sAccept = INVALID_SOCKET;

            // ★ 반드시 한 번 호출 (listen 소켓을 컨텍스트로 넘김)
            int one = 1;
            int rc = setsockopt(s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                reinterpret_cast<const char*>(&g_listen),
                sizeof(g_listen));
            if (rc == SOCKET_ERROR) {
                closesocket(s);
                // 에러 처리...
                break;
            }
            set_nodelay(s);
            associate_iocp((HANDLE)s);

            auto sess = std::make_shared<Session>();
            sess->s = s;
            sess->recvCtx.self = sess.get();

            // 세션 스냅샷에 추가
            for (;;) {
                auto oldv = std::atomic_load(&g_sessions);
                auto nv = std::make_shared<std::vector<SessionPtr>>(oldv ? *oldv : std::vector<SessionPtr>{});
                nv->push_back(sess);
                if (std::atomic_compare_exchange_weak(&g_sessions, &oldv, nv)) break;
            }

            // 첫 수신 예약
            ZeroMemory(&sess->recvCtx.ov, sizeof(OVERLAPPED));
            DWORD flags = 0, recvd = 0;
            int rc = WSARecv(s, &sess->recvCtx.wsa, 1, &recvd, &flags, &sess->recvCtx.ov, nullptr);
            if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                closesocket(s); remove_session(sess);
            }

            // 다음 AcceptEx 포스트
            delete ax;
            post_acceptex(1);
            break;
        }
        case OvType::Recv: {
            auto sess = static_cast<RecvCtx*>(base)->self;
            if (bytes == 0) {
                /*
                sess 가 가리키는 세션 객체를 전역 세션 리스트(g_sessions) 에서 제거하는 코드입니다.
                현재 IOCP 완료가 끝난 이 세션 객체를 전역 세션 벡터에서 찾아서 제거하라
                */
                remove_session(sess->shared_from_this());   //shared_from_this : 나 자신(sess)을 가르키는 shared_ptr 을 생성해라
                closesocket(sess->s);
            } else {
                sess->on_recv_completed(bytes);
            }
            break;
        }
        case OvType::Send: {
            auto* ctx = static_cast<SendCtx*>(base);
            auto sess = ctx->selfHold.get();
            sess->on_send_completed(ctx, bytes);
            break;
        }
        }
    }
}

//====================== AcceptEx 보일러 ======================
void post_acceptex(int nPosts) {
    for (int i = 0; i < nPosts; ++i) {
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
        s->enqueue_send(p); // per-session SPSC에 같은 shared_ptr 넣기 → zero-copy 확산
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
    std::puts("Clients can send lines; server broadcasts each complete line to all clients.");

    // 데모: 1초마다 서버가 'tick\n' 브로드캐스트
    std::thread ticker([] {
        for (;;) {
            auto msg = std::make_shared<ByteVec>();
            const char* s = "tick\n";
            msg->assign(s, s + 5);
            broadcast(msg);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });

    ticker.join();
    for (auto& t : workers) t.join();
    WSACleanup();
    return 0;
}
