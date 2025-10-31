#include "CIOCPServer.h"

//============================
CIOCPServer  __iocpSvr;
//============================

bool CIOCPServer::Start(char* ip, uint16_t port, int acceptDepth, int nWorkers)
{
    if (!init_winsock()) return false;

    m_is_stopping.store(false);
    m_acpt_depth = acceptDepth;

    m_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (!m_iocp_handle) { perrorW("CreateIoCompletionPort"); return false; }

    m_sock_listen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_sock_listen == INVALID_SOCKET) { perrorW("WSASocket listen"); return false; }

    sockaddr_in addr{};
    addr.sin_family         = AF_INET;
    //addr.sin_addr.s_addr    = inet_addr(ip);   //htonl(INADDR_ANY);
    addr.sin_addr.s_addr    = htonl(INADDR_ANY);
    addr.sin_port           = htons(port);

    if (::bind(m_sock_listen, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) { perrorW("bind"); return false; }
    if (listen(m_sock_listen, SOMAXCONN) == SOCKET_ERROR) { perrorW("listen"); return false; }

    associate_iocp((HANDLE)m_sock_listen);
    if (!ensure_extensions(m_sock_listen)) return false;

    // worker threads
    m_workthrd_cnt = nWorkers > 0 ? nWorkers : (std::max)(2u, std::thread::hardware_concurrency());
    m_vec_workthrd.reserve(m_workthrd_cnt);
    for (int i = 0; i < m_workthrd_cnt; ++i){
        //std::thread t([this]() { this->WorkerLoop();});
        //m_vec_workthrd.emplace_back(std::move(t));
        m_vec_workthrd.emplace_back([this] { WorkerLoop(); });
    }

    // AcceptEx depth
    post_AcceptEx(m_acpt_depth);

    return true;
}


void CIOCPServer::Stop() 
{
    bool expected = false;
    if (!m_is_stopping.compare_exchange_strong(expected, true)) return; // ����Ȱ� expected �� ������ return true, �ٸ��� return false;

    // 1) Listen �ݾƼ� ��� pending AcceptEx ���� �Ϸ� ����
    if (m_sock_listen != INVALID_SOCKET) {
        closesocket(m_sock_listen);
        m_sock_listen = INVALID_SOCKET;
    }

    // 2) ��� ���� ���� ����
    // - atomic_load: shared_ptr �� ���� ���� ���� API �� T* �� �Ķ���ͷ�, T�� ���ϰ�����
    // - ���� ������(shared_ptr) �� ���������� ����
    
    // std::shared_ptr<std::vector<SessionPtr>> snap = m_Sessions.load();
    // *snap => std::vector<SessionPtr>, �� ���͸� ����Ű�� shared_ptr �� ������ ���(���� ��ü)�̴�.

    auto snap = std::atomic_load(&m_Sessions);
    if (snap) {
        for (auto& s : *snap) {
            shutdown(s->m_sock, SD_BOTH);
            abort_close(s->m_sock);
        }
    }
    // 3) ��Ŀ �����
    for (int i = 0; i < m_workthrd_cnt; ++i)
        PostQueuedCompletionStatus(m_iocp_handle, 0, SHUTDOWN_KEY, nullptr);
}

void CIOCPServer::Join()
{
    for (auto& t : m_vec_workthrd) 
    {
        if (t.joinable()) 
            t.join();
    }
    m_vec_workthrd.clear();
    if (ticker_.joinable()) ticker_.join();
    if (m_iocp_handle) { CloseHandle(m_iocp_handle); m_iocp_handle = nullptr; }
}

// ������ ���Ƿ� ��ε�ĳ��Ʈ (ex. ticker, �˸� ��)
void CIOCPServer::broadcast_all_clients(const std::string& jsondata)
{
    auto msg = std::make_shared<TByteVec>();
    msg->insert(msg->end(), jsondata.begin(), jsondata.end());
    
    auto snap = std::atomic_load(&m_Sessions);
    if (!snap) return;
    for (auto& s : *snap) s->enqueue_SendTask(msg);
}

// ����ڰ� ���� ������ ����/���� �� ���ε�ĳ��Ʈ�ϰ� ���� �� hook
void CIOCPServer::SetOnLine(OnLine cb, void* user) 
{ 
    m_onLine_callback   = std::move(cb); 
    m_onLine_user       = user; 
}

// ����: ���� ���� ticker (���ϸ� ���)
void CIOCPServer::StartTicker(std::chrono::milliseconds period ) 
{
    ticker_ = std::thread([this, period] 
    {
        while (!m_is_stopping.load(std::memory_order_acquire)) 
        {
            auto msg = std::make_shared<TByteVec>();
            const char* s = "tick\r\n"; msg->assign(s, s + 6);
            //broadcast_all_clients(msg);
            std::this_thread::sleep_for(period);
        }
    });
}


    //====================== ���� ��ƿ��Ƽ ======================
void CIOCPServer::perrorW(const char* msg) {
    __common.log_fmt(ERR, "(%d)[%s]", (int)WSAGetLastError(), msg);
}

void CIOCPServer::set_nodelay(SOCKET s) {
    BOOL opt = TRUE; setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt));
}
void CIOCPServer::associate_iocp(HANDLE h, ULONG_PTR key) {
    if (!CreateIoCompletionPort(h, m_iocp_handle, key, 0)) { perrorW("CreateIoCompletionPort associate"); }
}
void CIOCPServer::abort_close(SOCKET s) {
    linger lg{ TRUE, 0 }; setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&lg, sizeof(lg));
    closesocket(s);
}


/*
    WinSock �� Ȯ���Լ�(Extension Function) - AcceptEx, GetAcceptExSockaddrs ���� �Լ����� ����ϱ� ����
    �Լ������͸� ���´�.
    - �Լ� ������ dll �� �־(mswsock.dll)

*/
bool CIOCPServer::ensure_extensions(SOCKET ls) 
{
    DWORD bytes = 0;
    GUID g1 = WSAID_ACCEPTEX, g2 = WSAID_GETACCEPTEXSOCKADDRS;

    if (WSAIoctl(ls, SIO_GET_EXTENSION_FUNCTION_POINTER, &g1, sizeof(g1),
        &m_acptex_ptr, sizeof(m_acptex_ptr), &bytes, NULL, NULL) == SOCKET_ERROR) {
        perrorW("WSAIoctl AcceptEx"); return false;
    }
    if (WSAIoctl(ls, SIO_GET_EXTENSION_FUNCTION_POINTER, &g2, sizeof(g2),
        &pGetAddrs_, sizeof(pGetAddrs_), &bytes, NULL, NULL) == SOCKET_ERROR) {
        perrorW("WSAIoctl GetAcceptExSockaddrs"); return false;
    }
    return true;
}
bool CIOCPServer::init_winsock() 
{
    if (m_winsock_inited) return true;
    
    WSADATA w; 
    if (WSAStartup(MAKEWORD(2, 2), &w) != 0) { 
        perrorW("WSAStartup"); 
        return false; 
    }
    
    m_winsock_inited = true; 
    return true;
}
void CIOCPServer::wsacleanup_if_inited() 
{
    if (m_winsock_inited) { 
        WSACleanup(); 
        m_winsock_inited = false; 
    }
}

//====================== AcceptEx ======================
void CIOCPServer::post_AcceptEx(int n) 
{
    if (m_is_stopping.load(std::memory_order_acquire)) return;

    for (int i = 0; i < n; ++i) 
    {
        auto* acptCtx       = new AcceptCtx();
        acptCtx->sAccept    = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
        if (acptCtx->sAccept == INVALID_SOCKET) { perrorW("WSASocket accept"); delete acptCtx; continue; }
    
        DWORD bytes = 0;
        BOOL ok = m_acptex_ptr(m_sock_listen, 
                                acptCtx->sAccept,
                                acptCtx->buf,
                                0,
                                sizeof(SOCKADDR_STORAGE) + 16, 
                                sizeof(SOCKADDR_STORAGE) + 16,
                                &bytes, 
                                &acptCtx->ov
                                );

        if (!ok && WSAGetLastError() != ERROR_IO_PENDING) { perrorW("AcceptEx"); delete acptCtx; }
    }
}

//====================== ���� ����(COW) ======================
void CIOCPServer::AddSession(const SessionPtr& newSession) 
{
    for (;;) 
    {
        auto oldVec = std::atomic_load(&m_Sessions);
        auto newVec = std::make_shared<std::vector<SessionPtr>>(oldVec ? *oldVec : std::vector<SessionPtr>{});
        newVec->push_back(newSession);

        /*
            bool atomic_compare_exchange_weak(
            std::atomic<T>*obj,                                 // the current value of an std::atomic
            typename std::atomic<T>::value_type * expected,     // provided non-atomic value
            typename std::atomic<T>::value_type desired         // if(obj==expected) obj=desired. return true;
            )
        */

        bool is_snapped_non_changed = std::atomic_compare_exchange_weak(&m_Sessions, &oldVec, newVec);
        if (is_snapped_non_changed)
            break;
    }
}

void CIOCPServer::RemoveSession(const SessionPtr& target) 
{
    for (;;) 
    {
        auto oldVec = std::atomic_load(&m_Sessions);
        if (!oldVec) return;

        auto newVec = std::make_shared<std::vector<SessionPtr>>(*oldVec);

        newVec->erase
        (
            std::remove_if
            (
                newVec->begin(),
                newVec->end(),
                [&](const SessionPtr& sp) { return sp.get() == target.get(); }
            ), 
                
            newVec->end()
        );

        bool is_snapped_non_changed = std::atomic_compare_exchange_weak(&m_Sessions, &oldVec, newVec);
        if (is_snapped_non_changed)
            break;
    }
}

//====================== ��Ŀ ======================
void CIOCPServer::WorkerLoop() 
{
    for (;;) 
    {
        DWORD bytes         = 0; 
        ULONG_PTR key       = 0; 
        LPOVERLAPPED pOvlap = nullptr;

        BOOL ok = GetQueuedCompletionStatus(m_iocp_handle, &bytes, &key, &pOvlap, INFINITE);

        // ���� ��Ƽ��
        if (pOvlap == nullptr && key == SHUTDOWN_KEY) break;

        if (!ok && pOvlap == nullptr) continue;

        // strand �½�ũ
        if (pOvlap && key == STRAND_KEY) 
        {
            auto* task = reinterpret_cast<TSendTask*>(pOvlap);
            task->selfSessionPtr->handle_SendTask(task->payload);
            delete task;
            continue;
        }

        //auto* base = TOvlapBase::From(pOvlap);
        auto* base = reinterpret_cast<TOvlapBase*>(pOvlap);
        switch (base->type) 
        {
        case TOvlapBase::Type::Accept: 
        {
            auto acptCtx      = std::unique_ptr< AcceptCtx>(static_cast<AcceptCtx*>(base));

            SOCKET sock       = acptCtx->sAccept;
            acptCtx->sAccept  = INVALID_SOCKET;

            // AcceptEx ���ؽ�Ʈ ������Ʈ
            int rc = setsockopt(sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                                reinterpret_cast<const char*>(&m_sock_listen), sizeof(m_sock_listen));
            if (rc == SOCKET_ERROR) {
                closesocket(sock);
                if (!m_is_stopping.load(std::memory_order_acquire)) post_AcceptEx(1);
                break;
            }
            set_nodelay(sock);
            associate_iocp((HANDLE)sock);

            // add session
            auto sess               = std::make_shared<TSession>();
            sess->m_sock            = sock;
            sess->m_iocp_ptr        = this;
            sess->m_recvCtx.self    = sess.get();

            AddSession(sess);

            __common.log_fmt(INFO, "[Accept Client](socket:%d)", sock);

            // ù ���� ����
            sess->PostRecv(sess, sock);
            //ZeroMemory(&sess->m_recvCtx.ov, sizeof(OVERLAPPED));
            //DWORD flags = 0, recvd = 0;
            //rc = WSARecv(sock, &sess->m_recvCtx.wsa, 1, &recvd, &flags, &sess->m_recvCtx.ov, nullptr);
            //if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
            //    RemoveSession(sess); 
            //    abort_close(sock);
            //}

            if (!m_is_stopping.load(std::memory_order_acquire)) post_AcceptEx(1);
            
            break;
        }
        case TOvlapBase::Type::Recv: {
            auto sess = static_cast<TRecvCtx*>(base)->self;
            
            // client socket closed
            if (bytes == 0) 
            { 
                __common.log_fmt(INFO, "client session will be deleted as socket[%d] was closed", sess->m_sock);
                RemoveSession(sess->shared_from_this()); abort_close(sess->m_sock);
            }
            else { sess->OnRecvCompleted(bytes); }
            break;
        }
        case TOvlapBase::Type::Send: {
            auto* ctx = static_cast<TSendCtx*>(base);
            ctx->selfSessionPtr->OnSendCompleted(ctx, bytes);
            break;
        }
        }
    }
}