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
#include "../../Common/CNoLockRingQueue.h"
#include "CGlobals.h"
#include <wincrypt.h>               //websocket
#include <json.hpp>

#pragma comment(lib, "Crypt32.lib") //websocket

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

//====================== 설정 상수 ======================

constexpr size_t MAX_LINE = 64 * 1024;           // 한 줄 최대 길이(DoS 방지)

#ifdef _WIN64
constexpr ULONG_PTR STRAND_KEY = 0xFFFF'FFFF'FFFF'FFFEULL;
constexpr ULONG_PTR SHUTDOWN_KEY = 0xFFFF'FFFF'FFFF'FFFDULL;

#else
constexpr ULONG_PTR STRAND_KEY = 990;
constexpr ULONG_PTR SHUTDOWN_KEY = 991;
#endif

//====================== 간단 SPSC 링버퍼 ======================


//====================== 공용 타입 ======================
enum class CLIENT_TP{NONE, Server, Client};
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
    void Join();    // workers join & clear. Stop() 이후 호출

    // 서버가 임의로 브로드캐스트 (ex. ticker, 알림 등)
    void broadcast_all_clients(const string& symbol, int tf, const std::string& jsondata);

    // 사용자가 수신 라인을 가공/필터 후 재브로드캐스트하고 싶을 때 hook
    void SetOnLine(OnLine cb, void* user = nullptr);// { m_onLine_callback = std::move(cb); m_onLine_user = user; }

    // 데모: 서버 내부 ticker (원하면 사용)
    void StartTicker(std::chrono::milliseconds period = std::chrono::seconds(1));

private:
    //====================== 내부 타입/컨텍스트 ======================
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
        std::enable_shared_from_this<T> 를 상속받은 클래스는
        이미 std::shared_ptr<T> 로 관리되고 있을 때,
        내부에서 shared_from_this() 를 호출하면
        자기자신을 가리키는 새로운 std::shared_ptr<T> 를 반환한다.
    */
    struct TSession : std::enable_shared_from_this<TSession>
    {
        SOCKET              m_sock{ INVALID_SOCKET };
        bool                m_is_websocket = false;     //websocket WS 업그레이드 완료 여부
        bool                m_ws_handshake_done = false;//websocket

        TRecvCtx            m_recvCtx{};
        std::string         m_buffer;

        std::atomic<bool>           m_is_sending{ false };
        CNoLockRingQueue<TPayLoad>  m_send_ringQ;

        string                      m_symbol_wanted;
        int                         m_tf_wanted;
        CLIENT_TP                   m_clientTp{CLIENT_TP::NONE};
        int                         m_my_idx{-1};

        CIOCPServer* m_iocp_ptr{};

        //websocket ---- WebSocket helpers ----
        bool try_handle_ws_handshake();            // 최초 수신에서 핸드셰이크 처리
        void handle_ws_data(const char* data, size_t len); // WS 프레임 파싱
        void ws_send_text(const std::string& s);   // 서버→클라 텍스트 프레임 전송
        void handle_ws_frames();
        void handle_plain_json_lines();
        bool parse_client_request(string rqst);
        

        void enqueue_SendTask(const TPayLoad& p);
        void handle_SendTask(const TPayLoad& p);
        void PostSend(TPayLoad&& p);
        void OnSendCompleted(TSendCtx* ctx, DWORD /*bytes*/);
        void OnRecvCompleted(DWORD bytes);
        bool PostRecv(SessionPtr sess, SOCKET& sock);
        void parse_client_request_payload(const nlohmann::json& j);
    };

    //====================== 내부 유틸리티 ======================
    void perrorW(const char* msg);
    static void set_nodelay(SOCKET s);
    void associate_iocp(HANDLE h, ULONG_PTR key = 0);
    void abort_close(SOCKET& s);
    bool ensure_extensions(SOCKET ls);
    bool init_winsock();
    void wsacleanup_if_inited();
    bool is_error_for_close(int socket_err);
    
    
    //====================== AcceptEx ======================
    void post_AcceptEx(int n);

    //====================== 세션 관리(COW) ======================
    void AddSession(const SessionPtr& newSession);
    void RemoveSession(const SessionPtr& target);

    //====================== 워커 ======================
    void WorkerLoop();


private:
    // winsock
    bool m_winsock_inited = false;

    // iocp/accept
    HANDLE  m_iocp_handle = nullptr;
    SOCKET  m_sock_listen = INVALID_SOCKET;
    LPFN_ACCEPTEX m_acptex_ptr = nullptr;
    LPFN_GETACCEPTEXSOCKADDRS pGetAddrs_ = nullptr; // 필요시 사용

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

// WebSocket 판단/유틸
bool looks_like_ws_handshake(const std::string& s);
bool parse_sec_websocket_key(const std::string& req, std::string& outKey);
bool compute_ws_accept(const std::string& key, std::string& outAccept); // SHA1+Base64

