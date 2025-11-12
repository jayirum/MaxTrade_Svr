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
    addr.sin_family = AF_INET;
    //addr.sin_addr.s_addr    = inet_addr(ip);   //htonl(INADDR_ANY);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (::bind(m_sock_listen, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) { perrorW("bind"); return false; }
    if (listen(m_sock_listen, SOMAXCONN) == SOCKET_ERROR) { perrorW("listen"); return false; }

    associate_iocp((HANDLE)m_sock_listen);
    if (!ensure_extensions(m_sock_listen)) return false;

    // worker threads
    m_workthrd_cnt = nWorkers > 0 ? nWorkers : (std::max)(2u, std::thread::hardware_concurrency());
    m_vec_workthrd.reserve(m_workthrd_cnt);
    for (int i = 0; i < m_workthrd_cnt; ++i) {
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
    if (!m_is_stopping.compare_exchange_strong(expected, true)) return; // 저장된게 expected 와 같으면 return true, 다르면 return false;

    // 1) Listen 닫아서 모든 pending AcceptEx 실패 완료 유도
    if (m_sock_listen != INVALID_SOCKET) {
        closesocket(m_sock_listen);
        m_sock_listen = INVALID_SOCKET;
    }

    // 2) 모든 세션 소켓 종료
    // - atomic_load: shared_ptr 에 대한 전용 원자 API 가 T* 를 파라미터로, T를 리턴값으로
    // - 현재 스냅샷(shared_ptr) 을 원자적으로 읽음

    // std::shared_ptr<std::vector<SessionPtr>> snap = m_Sessions.load();
    // *snap => std::vector<SessionPtr>, 즉 벡터를 가리키는 shared_ptr 의 역참조 결과(벡터 본체)이다.

    auto snap = std::atomic_load(&m_Sessions);
    if (snap) {
        for (auto& s : *snap) {
            s->m_send_ringQ.disable_ringQ();   //20251111-1
            shutdown(s->m_sock, SD_BOTH);
            abort_close(s->m_sock); s->m_sock = INVALID_SOCKET;
        }
    }
    // 3) 워커 깨우기
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

// 서버가 임의로 브로드캐스트 (ex. ticker, 알림 등)
void CIOCPServer::broadcast_all_clients(const string& symbol, int tf, const std::string& jsondata)
{
    auto msg = std::make_shared<TByteVec>();
    msg->insert(msg->end(), jsondata.begin(), jsondata.end());

    auto snap = std::atomic_load(&m_Sessions);
    if (!snap) return;
    for (auto& s : *snap)
    {
        if( s->m_clientTp==CLIENT_TP::Server ||
            (s->m_clientTp==CLIENT_TP::Client && 
             s->m_symbol_wanted.compare(symbol)==0 && 
             s->m_tf_wanted==tf 
            )
        ){
            s->enqueue_SendTask(msg);
        }
    }
}

// 사용자가 수신 라인을 가공/필터 후 재브로드캐스트하고 싶을 때 hook
void CIOCPServer::SetOnLine(OnLine cb, void* user)
{
    m_onLine_callback = std::move(cb);
    m_onLine_user = user;
}

// 데모: 서버 내부 ticker (원하면 사용)
void CIOCPServer::StartTicker(std::chrono::milliseconds period)
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


//====================== 내부 유틸리티 ======================
void CIOCPServer::perrorW(const char* msg) {
    __common.log_fmt(ERR, "(%d)[%s]", (int)WSAGetLastError(), msg);
}

void CIOCPServer::set_nodelay(SOCKET s) {
    BOOL opt = TRUE; setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt));
}
void CIOCPServer::associate_iocp(HANDLE h, ULONG_PTR key) {
    if (!CreateIoCompletionPort(h, m_iocp_handle, key, 0)) { perrorW("CreateIoCompletionPort associate"); }
}
void CIOCPServer::abort_close(SOCKET& s) {
    if (s == INVALID_SOCKET) return;
    linger lg{ TRUE, 0 }; setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&lg, sizeof(lg));
    closesocket(s); s = INVALID_SOCKET;
}


/*
    WinSock 의 확장함수(Extension Function) - AcceptEx, GetAcceptExSockaddrs 같은 함수들을 사용하기 위해
    함수포인터를 얻어온다.
    - 함수 구현이 dll 에 있어서(mswsock.dll)

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

bool CIOCPServer::is_error_for_close(int socket_err)
{
    bool should_close = false;

    switch (socket_err)
    {
    case WSAECONNRESET:
    case WSAESHUTDOWN:
    case WSAECONNABORTED:
    case WSAENOTSOCK:
    case WSAENETRESET:
    case ERROR_NETNAME_DELETED:
    case ERROR_CONNECTION_ABORTED:
    case ERROR_OPERATION_ABORTED:
        should_close = true;
        break;
    }
    return should_close;
}

//====================== AcceptEx ======================
void CIOCPServer::post_AcceptEx(int n)
{
    if (m_is_stopping.load(std::memory_order_acquire)) return;

    for (int i = 0; i < n; ++i)
    {
        auto* acptCtx = new AcceptCtx();
        acptCtx->sAccept = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
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

//====================== 세션 관리(COW) ======================
void CIOCPServer::AddSession(const SessionPtr& newSession)
{
    for (;;)
    {
        auto oldVec = std::atomic_load(&m_Sessions);
        int size = (oldVec && !oldVec->empty())? static_cast<int>(oldVec->size()) : 0;
        newSession->m_my_idx = size;

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
        if (is_snapped_non_changed){
            __common.log_fmt(INFO, "[Accept Client](Total:%d)(Client-Idx:%d)(socket:%d)", size+1, size, newSession->m_sock);
            break;
        }
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
        if (is_snapped_non_changed){
            int size = (newVec && !newVec->empty()) ? static_cast<int>(newVec->size()) : 0;
            __common.log_fmt(INFO, "[RemoveSession](Total Size:%d)(Client-Idx:%d)(Socket:%d)", 
                                size, target->m_my_idx, target->m_sock);

            abort_close(target->m_sock);
            break;
        }
    }
}

//====================== 워커 ======================
void CIOCPServer::WorkerLoop()
{
    for (;;)
    {
        DWORD bytes = 0;
        ULONG_PTR key = 0;
        LPOVERLAPPED pOvlap = nullptr;

        BOOL ok = GetQueuedCompletionStatus(m_iocp_handle, &bytes, &key, &pOvlap, INFINITE);

        // 종료 센티넬
        if (pOvlap == nullptr && key == SHUTDOWN_KEY) break;

        if (!ok && pOvlap == nullptr) continue;

        // strand 태스크
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
            auto acptCtx = std::unique_ptr< AcceptCtx>(static_cast<AcceptCtx*>(base));

            SOCKET sock = acptCtx->sAccept;
            acptCtx->sAccept = INVALID_SOCKET;

            // AcceptEx 컨텍스트 업데이트
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
            auto sess = std::make_shared<TSession>();
            sess->m_sock = sock;
            sess->m_iocp_ptr = this;
            sess->m_recvCtx.self = sess.get();

            AddSession(sess);

            // 첫 수신 예약
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
                //__common.log_fmt(INFO, "client session will be deleted as socket[%d] was closed", sess->m_sock);
                //abort_close(sess->m_sock);
                RemoveSession(sess->shared_from_this()); 
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

// ws handshake 인지 확인하고, 그렇다면 서버에서 handshake에 대한 응답 전송
bool CIOCPServer::TSession::try_handle_ws_handshake()
{
    // m_buffer에 최초 HTTP 요청이 누적됨 (OnRecvCompleted에서 append)
    // 헤더 끝(\r\n\r\n) 확인 전에는 더 받는다.
    auto hdrEnd = m_buffer.find("\r\n\r\n");
    if (hdrEnd == std::string::npos) return false; // 아직 다 안 옴

    std::string req = m_buffer.substr(0, hdrEnd + 4);
    if (!looks_like_ws_handshake(req)) return false;

    std::string key, accept;
    if (!parse_sec_websocket_key(req, key)) return false;
    if (!compute_ws_accept(key, accept)) return false;

    // 101 응답
    std::string resp = "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: " + accept + "\r\n"
        "\r\n";

    auto msg = std::make_shared<TByteVec>(resp.begin(), resp.end());
    enqueue_SendTask(msg);

    // 버퍼에서 헤더 제거(혹시 남은 데이터는 이후 프레임으로 간주)
    m_buffer.erase(0, hdrEnd + 4);
    m_is_websocket = true;
    m_ws_handshake_done = true;
    return true;
}

// 아주 단순한 WS 프레임 파서 (텍스트 단일프레임, ping/close 처리)
// 실제 서비스에선 분할/대용량/연속 프레임, 확장, 대용량 마스킹 최적화 필요
void CIOCPServer::TSession::handle_ws_data(const char* data, size_t len)
{
    size_t i = 0;
    while (len - i >= 2) {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(data) + i;
        uint8_t b0 = p[0], b1 = p[1];
        bool fin = (b0 & 0x80) != 0;
        uint8_t opcode = (b0 & 0x0F);
        bool masked = (b1 & 0x80) != 0;
        uint64_t paylen = (b1 & 0x7F);
        size_t hdr = 2;

        if (!masked) { // 브라우저→서버는 반드시 마스킹
            // 프로토콜 위반: 연결 종료
            shutdown(m_sock, SD_BOTH); return;
        }

        if (paylen == 126) { if (len - i < hdr + 2) return; paylen = (p[2] << 8) | p[3]; hdr += 2; }
        else if (paylen == 127) {
            if (len - i < hdr + 8) return;
            paylen = 0;
            for (int k = 0; k < 8; ++k) paylen = (paylen << 8) | p[2 + k];
            hdr += 8;
        }

        if (len - i < hdr + 4 + paylen) return; // 불완전: 더 받기
        uint8_t mkey[4] = { p[hdr + 0], p[hdr + 1], p[hdr + 2], p[hdr + 3] };
        const uint8_t* payload = p + hdr + 4;

        std::string out; out.resize((size_t)paylen);
        for (size_t j = 0; j < (size_t)paylen; ++j) out[j] = char(payload[j] ^ mkey[j & 3]);

        if (opcode == 0x1) { // text
            __common.log_fmt(INFO, "[WS TEXT]%s", out.c_str());
            // echo 데모 (원하면 브로드캐스트 등으로 변경)
            ws_send_text(out);
        }
        else if (opcode == 0x9) { // ping -> pong
            // pong 전송
            // FIN+PONG, payload 그대로
            std::string frame;
            frame.push_back((char)0x8A);
            if (out.size() <= 125) { frame.push_back((char)out.size()); }
            else { /* 단순화: 작은 핑만 지원 */ frame.push_back(0); }
            frame.append(out);
            auto msg = std::make_shared<TByteVec>(frame.begin(), frame.end());
            enqueue_SendTask(msg);
        }
        else if (opcode == 0x8) { // close
            shutdown(m_sock, SD_BOTH);
            return;
        }
        // (binary/continuation 등은 필요시 확장)

        i += hdr + 4 + (size_t)paylen;
    }
}

void CIOCPServer::TSession::ws_send_text(const std::string& s)
{
    // 서버→클라는 마스킹하지 않음
    std::string frame;
    frame.push_back((char)0x81); // FIN + text
    if (s.size() <= 125) { frame.push_back((char)s.size()); }
    else if (s.size() <= 0xFFFF) {
        frame.push_back(126);
        frame.push_back((char)((s.size() >> 8) & 0xFF));
        frame.push_back((char)(s.size() & 0xFF));
    }
    else {
        frame.push_back(127);
        for (int k = 7; k >= 0; --k) frame.push_back((char)(((uint64_t)s.size() >> (k * 8)) & 0xFF));
    }
    frame.append(s);
    auto msg = std::make_shared<TByteVec>(frame.begin(), frame.end());
    enqueue_SendTask(msg);
}


void CIOCPServer::TSession::OnRecvCompleted(DWORD bytes)
{
    if (bytes == 0) { /* peer closed */ return; }

    m_buffer.append(m_recvCtx.buf, m_recvCtx.buf + bytes);

    // 1) 아직 WS 아닌 상태라면, 핸드셰이크 시도
    if (!m_is_websocket) 
    {
        // // ws handshake 인지 확인하고, 그렇다면 서버에서 handshake에 대한 응답 전송
        // 먼저 WS 핸드셰이크가 가능한지 확인 (헤더가 다 올 때까지 기다림)
        if (try_handle_ws_handshake())  // return true 이면 m_is_websocket=true 됨
        {
            // 핸드셰이크 성공 시, try_handle_ws_handshake 내부에서
            // m_buffer에서 HTTP 헤더를 제거했으므로
            // 남은 바이트는 WS 프레임으로 간주하여 처리
            handle_ws_frames();
        }
        else 
        {
            // 아직 WS가 아니라면, 플레인 TCP JSON 프로토콜 처리
            // => 줄 구분(NDJSON) 방식으로 완전한 라인만 파싱
            handle_plain_json_lines();
        }
    }
    else 
    {
        // 이미 WS 모드: 프레임이 완성될 때까지 m_buffer를 소비
        handle_ws_frames();        
    }

    // 다음 수신 예약
    PostRecv(shared_from_this(), m_sock);
}

void CIOCPServer::TSession::enqueue_SendTask(const TPayLoad& p)
{
    auto self = shared_from_this();
    auto* task = new TSendTask(self, p);
    PostQueuedCompletionStatus(m_iocp_ptr->m_iocp_handle, 0, STRAND_KEY, reinterpret_cast<LPOVERLAPPED>(task));
    //__common.debug_fmt("<enqueue_SendTask>(sock:%d)(%s)", self->m_sock, p->data());
}
void CIOCPServer::TSession::handle_SendTask(const TPayLoad& p)
{
    if (!m_send_ringQ.is_available()) return;

    RING_Q_RET ret_code;
    while ((ret_code = m_send_ringQ.push(p)) == RING_Q_RET::Fail) _mm_pause();

    //return the value before the call
    if (m_is_sending.exchange(true, std::memory_order_acq_rel)) return;

    TPayLoad newP;
    if ((ret_code = m_send_ringQ.pop(newP)) == RING_Q_RET::Fail) {
        m_is_sending.store(false, std::memory_order_release);
        return;
    }

    PostSend(std::move(newP));
}

void CIOCPServer::TSession::PostSend(TPayLoad&& p)
{
    if (!p || p->empty()) {
        __common.log_fmt(ERR, "PayLoad is null or empty");
        return;
    }

    auto* ctx = new TSendCtx(shared_from_this(), std::move(p));
    assert(ctx->payload);                // ② 디버그 가드
    const DWORD len = static_cast<DWORD>(ctx->payload->size());
    if (len == 0) { delete ctx; return; }

    WSABUF wsa{ len, const_cast<char*>(ctx->payload->data()) };

    DWORD sent = 0;
    int rc = WSASend(m_sock, &wsa, 1, &sent, 0, &ctx->ov, nullptr);
    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
    {
        delete ctx;
        m_is_sending.store(false, std::memory_order_release);
        __common.log_fmt(ERR, "[PostSend Error:%d](%d)", m_sock, WSAGetLastError());

        if (m_iocp_ptr->is_error_for_close(WSAGetLastError())) {
            //m_iocp_ptr->abort_close(m_sock);
            m_iocp_ptr->RemoveSession(shared_from_this());
        }
    }
}
void CIOCPServer::TSession::OnSendCompleted(TSendCtx* ctx, DWORD /*bytes*/)
{
    delete ctx;
    TPayLoad next;
    if (m_send_ringQ.pop(next) == RING_Q_RET::Succ) {
        PostSend(std::move(next));
    }
    else
    {
        m_is_sending.store(false, std::memory_order_release);
        if (m_send_ringQ.pop(next) == RING_Q_RET::Succ && !m_is_sending.exchange(true, std::memory_order_acq_rel))
            PostSend(std::move(next));
    }
}

bool CIOCPServer::TSession::PostRecv(SessionPtr sess, SOCKET& sock)
{
    ZeroMemory(&sess->m_recvCtx.ov, sizeof(OVERLAPPED));
    DWORD flags = 0, recvd = 0;
    int rc = WSARecv(sock, &sess->m_recvCtx.wsa, 1, &recvd, &flags, &sess->m_recvCtx.ov, nullptr);
    if (rc == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
        __common.log_fmt(ERR, "[PostRecv Error](%d)", WSAGetLastError());
        if (m_iocp_ptr->is_error_for_close(WSAGetLastError())) {
            //m_iocp_ptr->abort_close(m_sock);
            m_iocp_ptr->RemoveSession(shared_from_this());
        }
        return false;
    }
    return true;
}

bool CIOCPServer::TSession::parse_client_request(string rqst)
{
    if (!nlohmann::json::accept(rqst)) {
        __common.log_fmt(ERR, "[parse_client_request] json 데이터 파싱 오류(%s)", rqst.c_str());
        // rqst에 뭐가 들어왔는지 길이/헥사로 확인
        std::cerr << "Invalid JSON. size=" << rqst.size() << "\n";
        for (unsigned char c : rqst) 
            std::cerr << std::hex << (int)c << ' ';
        std::cerr << std::dec << "\n";
        return false;
    }

    nlohmann::json j = nlohmann::json::parse(rqst);

    // 접근
    m_symbol_wanted = j["symbol"];
    m_tf_wanted     = j["timeframe"];

    string type     = j["client_type"];
    CStringUtils u; u.tolower_str(type);

    m_clientTp = (type =="server")? CLIENT_TP::Server : CLIENT_TP::Client;

    __common.log_fmt(INFO, "client 요청 SYMBOL(%s) Timeframe(%d) ClientType(%s)", 
                    m_symbol_wanted.c_str(), m_tf_wanted, type.c_str());
    return true;
}


void CIOCPServer::TSession::handle_ws_frames()
{
    // RFC 6455 최소 헤더 2바이트
    while (m_buffer.size() >= 2)
    {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(m_buffer.data());

        // 첫 바이트
        bool fin = (p[0] & 0x80) != 0;
        unsigned opcode = (p[0] & 0x0F);      // 0x1 = text, 0x2 = binary, 0x8 close, 0x9 ping, 0xA pong
        // 두 번째 바이트
        bool masked = (p[1] & 0x80) != 0;
        uint64_t plen = (p[1] & 0x7F);

        size_t pos = 2;

        if (plen == 126) {
            if (m_buffer.size() < pos + 2) return; // 더 받기
            plen = (uint64_t(p[pos]) << 8) | uint64_t(p[pos + 1]);
            pos += 2;
        }
        else if (plen == 127) {
            if (m_buffer.size() < pos + 8) return; // 더 받기
            plen = 0;
            for (int i = 0; i < 8; ++i) plen = (plen << 8) | uint64_t(p[pos + i]);
            pos += 8;
        }

        uint32_t mask = 0;
        if (masked) {
            if (m_buffer.size() < pos + 4) return; // 더 받기
            mask = (uint32_t(p[pos]) << 24) | (uint32_t(p[pos + 1]) << 16) |
                (uint32_t(p[pos + 2]) << 8) | uint32_t(p[pos + 3]);
            pos += 4;
        }

        // 총 프레임 길이 확인
        uint64_t frame_len = pos + plen;
        if (m_buffer.size() < frame_len) return; // 프레임 미완성 → 더 받기

        // 페이로드 추출
        std::string payload;
        payload.resize(static_cast<size_t>(plen));
        if (plen)
            memcpy(payload.data(), m_buffer.data() + pos, static_cast<size_t>(plen));

        // 마스크 해제
        if (masked && plen) {
            const unsigned char* mkey = reinterpret_cast<const unsigned char*>(&mask);
            for (size_t i = 0; i < payload.size(); ++i)
                payload[i] = payload[i] ^ mkey[i & 3];
        }

        // 소비
        m_buffer.erase(0, static_cast<size_t>(frame_len));

        // 컨트롤/플래그 처리
        if (!fin) {
            // (단순 구현) fragmented 는 여기선 미지원 → 필요 시 조각 누적 로직 추가
            continue;
        }

        switch (opcode)
        {
        case 0x1: // text
        {
            // 텍스트 JSON만 처리
            if (!nlohmann::json::accept(payload)) { 
                //TODO log_bad_json(payload); 
                break; 
            }
            auto j = nlohmann::json::parse(payload, nullptr, false);
            if (j.is_discarded()) { 
                //TODO log_bad_json(payload); 
                break; 
            }
            parse_client_request_payload(j);
            break;
        }
        case 0x8: // close
            //TODO send_ws_close_and_shutdown();
            return;
        case 0x9: // ping
            //TODO send_ws_pong(); // 필요 시 구현
            break;
        case 0xA: // pong
            // no-op
            break;
        default:
            // 다른 opcode 무시/로그
            break;
        }
    }
}

void CIOCPServer::TSession::handle_plain_json_lines()
{
    for (;;)
    {
        size_t nl = m_buffer.find('\n');
        if (nl == std::string::npos) break; // 한 줄이 완성될 때까지 더 받기

        std::string one = m_buffer.substr(0, nl);
        // 윈도우 클라이언트면 \r\n 일 수 있으니 \r 제거
        if (!one.empty() && one.back() == '\r') one.pop_back();

        m_buffer.erase(0, nl + 1);

        if (one.empty()) continue;

        // 안전 검사 후 파싱
        if (!nlohmann::json::accept(one)) { 
            //TODO log_bad_json(one); 
            continue; 
        }

        auto j = nlohmann::json::parse(one, nullptr, false);
        if (j.is_discarded()) { 
            //TODO log_bad_json(one); 
            continue; 
        }

        parse_client_request_payload(j);   // JSON 객체 중심 처리 함수(아래)
    }
}

void CIOCPServer::TSession::parse_client_request_payload(const nlohmann::json& j)
{
    // 안전 접근 예시
    // ex) {"symbol":"all","timeframe":0,"client_type":"server"}
    try {
        if (!j.is_object()) { 
            //TODO log_bad_json(j.dump()); 
            return; 
        }

        // 필요한 필드만 체크
        m_symbol_wanted = j.value("symbol", "");
        m_tf_wanted     = j.value("timeframe", 0);

        std::string client_type = j.value("client_type", "");
        CStringUtils u; u.tolower_str(client_type);

        m_clientTp = (client_type == "server") ? CLIENT_TP::Server : CLIENT_TP::Client;

        __common.log_fmt(INFO, "client 요청 SYMBOL(%s) Timeframe(%d) ClientType(%s)",
            m_symbol_wanted.c_str(), m_tf_wanted, client_type.c_str());
    }
    catch (const std::exception& e) {
        __common.log_fmt(ERR, "JSON handling error: %s", e.what());
    }
}



static inline std::string tolower_str(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}

bool looks_like_ws_handshake(const std::string& s) {
    // 최소: "GET "로 시작하고 헤더에 Upgrade: websocket, Connection: Upgrade, Sec-WebSocket-Key 존재
    if (s.rfind("GET ", 0) != 0) return false;
    auto ls = tolower_str(s);
    return (ls.find("upgrade: websocket") != std::string::npos) &&
        (ls.find("connection: upgrade") != std::string::npos) &&
        (ls.find("sec-websocket-key:") != std::string::npos);
}

bool parse_sec_websocket_key(const std::string& req, std::string& outKey) {
    auto ls = tolower_str(req);
    auto pos = ls.find("sec-websocket-key:");
    if (pos == std::string::npos) return false;
    // 그 줄의 끝까지
    auto end = req.find("\r\n", pos);
    if (end == std::string::npos) return false;
    // 원문에서 앞뒤 공백 제거
    std::string line = req.substr(pos, end - pos);
    auto colon = line.find(':');
    if (colon == std::string::npos) return false;
    std::string v = line.substr(colon + 1);
    // trim
    auto l = v.find_first_not_of(" \t"); if (l == std::string::npos) return false;
    auto r = v.find_last_not_of(" \t");  if (r == std::string::npos) return false;
    outKey = v.substr(l, r - l + 1);
    return !outKey.empty();
}

bool compute_ws_accept(const std::string& key,  // 클라이언트가 보낸 Sec-WebSocket-Key 문자열(Base64 형태)
    std::string& outAccept) // 서버가 보낼 Sec-WebSocket-Accept 문자열(Base64 형태)
{
    static const char* GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string src = key; src += GUID;

    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    BYTE sha[20]; DWORD shaLen = sizeof(sha);

    if (!CryptAcquireContextA(&hProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return false;
    if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash)) { CryptReleaseContext(hProv, 0); return false; }
    if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(src.data()), (DWORD)src.size(), 0)) {
        CryptDestroyHash(hHash); CryptReleaseContext(hProv, 0); return false;
    }
    if (!CryptGetHashParam(hHash, HP_HASHVAL, sha, &shaLen, 0)) {
        CryptDestroyHash(hHash); CryptReleaseContext(hProv, 0); return false;
    }
    CryptDestroyHash(hHash); CryptReleaseContext(hProv, 0);

    // Base64
    DWORD outLen = 0;
    if (!CryptBinaryToStringA(sha, shaLen, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &outLen)) return false;
    std::string b64(outLen, '\0');
    if (!CryptBinaryToStringA(sha, shaLen, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, b64.data(), &outLen)) return false;
    b64.resize(outLen);
    outAccept = std::move(b64);
    return true;
}
