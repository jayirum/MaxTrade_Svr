/*
std::atomic<
            std::shared_ptr<
                            std::vector<
                                        SessionPtr (std::shared_ptr<TSession>)  >>> m_Sessions;


*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
#include <functional>
#include "../../Common/CspscRing.h"
#include "CGlobals.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

//====================== ���� ��� ======================

constexpr size_t MAX_LINE = 64 * 1024;           // �� �� �ִ� ����(DoS ����)

#ifdef _WIN64
constexpr ULONG_PTR STRAND_KEY = 0xFFFF'FFFF'FFFF'FFFEULL;
constexpr ULONG_PTR SHUTDOWN_KEY = 0xFFFF'FFFF'FFFF'FFFDULL;

#else
constexpr ULONG_PTR STRAND_KEY   = 990;
constexpr ULONG_PTR SHUTDOWN_KEY = 991;
#endif

//====================== ���� SPSC ������ ======================


//====================== ���� Ÿ�� ======================
using TByteVec = std::vector<char>;
using TPayLoad = std::shared_ptr<const TByteVec>;

class CIOCPServer 
{

public:
    
    using OnLine = std::function<void(const std::string& line, void* user)>;

    CIOCPServer() = default;
    ~CIOCPServer() { Stop(); Join(); wsacleanup_if_inited(); }

    // ---------- Public API ----------
    bool Start(char* ip, uint16_t port, int acceptDepth = 64, int nWorkers = 0);
    void Stop();
    void Join();    // workers join & clear. Stop() ���� ȣ��

    // ������ ���Ƿ� ��ε�ĳ��Ʈ (ex. ticker, �˸� ��)
    void broadcast_all_clients(const std::string& jsondata) ;

    // ����ڰ� ���� ������ ����/���� �� ���ε�ĳ��Ʈ�ϰ� ���� �� hook
    void SetOnLine(OnLine cb, void* user = nullptr);// { m_onLine_callback = std::move(cb); m_onLine_user = user; }

    // ����: ���� ���� ticker (���ϸ� ���)
    void StartTicker(std::chrono::milliseconds period = std::chrono::seconds(1));

private:
    //====================== ���� Ÿ��/���ؽ�Ʈ ======================
    struct TOvlapBase {
        OVERLAPPED ov{}; 
        enum class Type { Accept, Recv, Send } type{};
        explicit TOvlapBase(Type t) : type(t) { 
            //static_assert(offsetof(TOvlapBase, ov) == 0, "OVERLAPPED must be first");
            ZeroMemory(&ov, sizeof(ov)); 
        }
        static TOvlapBase* From(LPOVERLAPPED p) { return reinterpret_cast<TOvlapBase*>(p); }
        //virtual ~TOvlapBase(){}
    };
    struct AcceptCtx : TOvlapBase {
        SOCKET sAccept{ INVALID_SOCKET };
        char buf[(sizeof(SOCKADDR_STORAGE) + 16) * 2]{};
        AcceptCtx() : TOvlapBase(Type::Accept) {}
    };
    struct TSession;
    using SessionPtr = std::shared_ptr<TSession>;

    struct TRecvCtx : TOvlapBase {
        WSABUF wsa{}; char buf[16 * 1024]; TSession* self{};
        TRecvCtx() : TOvlapBase(Type::Recv) { wsa.buf = buf; wsa.len = sizeof(buf); }
    };
    struct TSendCtx : TOvlapBase {
        SessionPtr selfSessionPtr; TPayLoad payload;
        TSendCtx(SessionPtr s, TPayLoad p) : TOvlapBase(Type::Send), selfSessionPtr(std::move(s)), payload(std::move(p)) {}
    };
    struct TSendTask : OVERLAPPED 
    {
        SessionPtr selfSessionPtr; 
        TPayLoad payload;
        TSendTask(SessionPtr s, TPayLoad p) : selfSessionPtr(std::move(s)), payload(std::move(p)) { ZeroMemory(this, sizeof(OVERLAPPED)); }
    };

    /*
        std::enable_shared_from_this<T> �� ��ӹ��� Ŭ������ 
        �̹� std::shared_ptr<T> �� �����ǰ� ���� ��,
        ���ο��� shared_from_this() �� ȣ���ϸ� 
        �ڱ��ڽ��� ����Ű�� ���ο� std::shared_ptr<T> �� ��ȯ�Ѵ�.
    */
    struct TSession : std::enable_shared_from_this<TSession> 
    {
        SOCKET              m_sock{ INVALID_SOCKET };
        
        TRecvCtx            m_recvCtx{};
        std::string         m_buffer;
        
        std::atomic<bool>   m_is_sending{ false };
        CspscRing<TPayLoad>  m_sendQ;
    
        CIOCPServer*        m_iocp_ptr{};

        void enqueue_SendTask(const TPayLoad& p) 
        {
            auto self   = shared_from_this();
            auto* task  = new TSendTask(self, p);
            PostQueuedCompletionStatus(m_iocp_ptr->m_iocp_handle, 0, STRAND_KEY, reinterpret_cast<LPOVERLAPPED>(task));
            //__common.debug_fmt("<enqueue_SendTask>(sock:%d)(%s)", self->m_sock, p->data());
        }
        void handle_SendTask(const TPayLoad& p)
        {
            while (!m_sendQ.push(p)) _mm_pause();
            
            //kickSend_if_idle();
            if (m_is_sending.exchange(true, std::memory_order_acq_rel)) return;

            TPayLoad newP;
            if (!m_sendQ.pop(newP)) {
                m_is_sending.store(false, std::memory_order_release); 
                return; 
            }
            //__common.debug_fmt("<handle_SendTask> PostSend to Client(sock:%d)(%s)", m_sock, newP->data());
            PostSend(std::move(newP));
        }

        void PostSend(TPayLoad&& p) 
        {
            auto* ctx = new TSendCtx(shared_from_this(), std::move(p));
            WSABUF wsa{ (ULONG)ctx->payload->size(), const_cast<char*>(ctx->payload->data()) };
            
            DWORD sent = 0;
            int rc = WSASend(m_sock, &wsa, 1, &sent, 0, &ctx->ov, nullptr);
            if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) 
            {
                delete ctx; 
                m_is_sending.store(false, std::memory_order_release);
                //m_iocp_ptr->RemoveSession(shared_from_this());   //TODO. �����ؾ� �ϳ� �α�
                //m_iocp_ptr->abort_close(m_sock);
                __common.log_fmt(ERR,"[PostSend Error](%d)", WSAGetLastError);
            }
        }
        void OnSendCompleted(TSendCtx* ctx, DWORD /*bytes*/) 
        {
            delete ctx;
            TPayLoad next;
            if (m_sendQ.pop(next)) {
                PostSend(std::move(next));
            }
            else 
            {
                m_is_sending.store(false, std::memory_order_release);
                if (m_sendQ.pop(next) && !m_is_sending.exchange(true, std::memory_order_acq_rel))
                    PostSend(std::move(next));
            }
        }
        void OnRecvCompleted(DWORD bytes) 
        {
            m_buffer.append(m_recvCtx.buf, m_recvCtx.buf + bytes);
            __common.log_fmt(INFO, "[RECV FROM CLIENT](%s)", m_buffer.c_str());
            m_buffer.clear();
            //m_buffer.append(m_recvCtx.buf, m_recvCtx.buf + bytes);
            //if (m_buffer.size() > MAX_LINE) { 
            //    m_iocp_ptr->RemoveSession(shared_from_this());  //TODO
            //    m_iocp_ptr->abort_close(m_sock); 
            //    return; 
            //}

            //size_t pos = 0;
            //for (;;) {
            //    auto eol = m_buffer.find("\r\n", pos);
            //    if (eol == std::string::npos) break;
            //    std::string line = m_buffer.substr(pos, eol - pos);
            //    pos = eol + 2;

            //    //if (m_iocp_ptr->m_onLine_callback) m_iocp_ptr->m_onLine_callback(line, m_iocp_ptr->m_onLine_user);
            //    //TODO LOGGING

            //    //auto msg = std::make_shared<TByteVec>();
            //    //msg->insert(msg->end(), line.begin(), line.end());
            //    //msg->push_back('\r'); msg->push_back('\n');
            //    //m_iocp_ptr->broadcast_all_clients(msg);
            //}
            //m_buffer.erase(0, pos);

            PostRecv(shared_from_this(), m_sock);
        }

        bool PostRecv(SessionPtr sess, SOCKET& sock)
        {
            ZeroMemory(&sess->m_recvCtx.ov, sizeof(OVERLAPPED));
            DWORD flags = 0, recvd = 0;
            int rc = WSARecv(sock, &sess->m_recvCtx.wsa, 1, &recvd, &flags, &sess->m_recvCtx.ov, nullptr);
            if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
                //sess->m_iocp_ptr->RemoveSession(sess);
                //sess->m_iocp_ptr->abort_close(sock);
                __common.log_fmt(ERR, "[PostRecv Error](%d)", WSAGetLastError);
                return false;
            }
            return true;
        }
    };

    //====================== ���� ��ƿ��Ƽ ======================
    void perrorW(const char* msg);
    static void set_nodelay(SOCKET s);
    void associate_iocp(HANDLE h, ULONG_PTR key = 0);
    void abort_close(SOCKET s);
    bool ensure_extensions(SOCKET ls);
    bool init_winsock();
    void wsacleanup_if_inited();

    //====================== AcceptEx ======================
    void post_AcceptEx(int n) ;

    //====================== ���� ����(COW) ======================
    void AddSession(const SessionPtr& newSession) ;
    void RemoveSession(const SessionPtr& target);

    //====================== ��Ŀ ======================
    void WorkerLoop();
    

private:
    // winsock
    bool m_winsock_inited = false;

    // iocp/accept
    HANDLE  m_iocp_handle = nullptr;
    SOCKET  m_sock_listen = INVALID_SOCKET;
    LPFN_ACCEPTEX m_acptex_ptr = nullptr;
    LPFN_GETACCEPTEXSOCKADDRS pGetAddrs_ = nullptr; // �ʿ�� ���

    std::atomic<bool>                       m_is_stopping{ false };
    int m_acpt_depth = 64;

    // workers/ticker
    int m_workthrd_cnt = 0;
    std::vector<std::thread>                m_vec_workthrd;
    std::thread                             ticker_;

    // sessions(COW - copy-on-write)
    std::atomic<std::shared_ptr<std::vector<SessionPtr>>> m_Sessions;   //SessionPtr = std::shared_ptr<TSession>)

    // callbacks
    OnLine m_onLine_callback{};
    void* m_onLine_user{ nullptr };
};

//============================
extern CIOCPServer  __iocpSvr;
//============================

//====================== ��� ��(���� main) ======================
// ���� �ۿ����� �Ʒ� main ���, ���� ���ø����̼��� ����������
// CIOCPServer �ν��Ͻ� ���� �� Start �� �ʿ� �� broadcast_all_clients �� Stop/Join ȣ�⸸ �ϸ� �˴ϴ�.
#ifdef m_iocp_handleSERVER_DEMO_MAIN
int main() {
    CIOCPServer svr;
    if (!svr.Start(9000)) return 1;

    // ���� ���� �ݹ�(�ɼ�)
    svr.SetOnLine([](const std::string& line, void*) {
        // ���⼭ ���ø����̼� ������ ����
        // ��: Ư�� ��� ó��, �α� ��
        // printf("[onLine] %s\n", line.c_str());
        });

    svr.StartTicker(); // �ɼ�: 1�ʸ��� "tick\r\n" ��ε�ĳ��Ʈ

    puts("IOCP server on 9000. Press Enter to stop.");
    getchar();

    svr.Stop();
    svr.Join();
    return 0;
}
#endif
