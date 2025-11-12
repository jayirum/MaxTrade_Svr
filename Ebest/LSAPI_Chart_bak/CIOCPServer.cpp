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
void CIOCPServer::broadcast_all_clients(const std::string& jsondata)
{
    auto msg = std::make_shared<TByteVec>();
    msg->insert(msg->end(), jsondata.begin(), jsondata.end());
    
    auto snap = std::atomic_load(&m_Sessions);
    if (!snap) return;
    for (auto& s : *snap) s->enqueue_SendTask(msg);
}

// 사용자가 수신 라인을 가공/필터 후 재브로드캐스트하고 싶을 때 hook
void CIOCPServer::SetOnLine(OnLine cb, void* user) 
{ 
    m_onLine_callback   = std::move(cb); 
    m_onLine_user       = user; 
}

// 데모: 서버 내부 ticker (원하면 사용)
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
    if(s==INVALID_SOCKET) return;
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

//====================== 세션 관리(COW) ======================
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
    __common.log_fmt(INFO, "[Remove Session](%d)", target->m_sock);
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

//====================== 워커 ======================
void CIOCPServer::WorkerLoop() 
{
    for (;;) 
    {
        DWORD bytes         = 0; 
        ULONG_PTR key       = 0; 
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
            auto acptCtx      = std::unique_ptr< AcceptCtx>(static_cast<AcceptCtx*>(base));

            SOCKET sock       = acptCtx->sAccept;
            acptCtx->sAccept  = INVALID_SOCKET;

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
            auto sess               = std::make_shared<TSession>();
            sess->m_sock            = sock;
            sess->m_iocp_ptr        = this;
            sess->m_recvCtx.self    = sess.get();

            AddSession(sess);

            __common.log_fmt(INFO, "[Accept Client](socket:%d)", sock);

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
    m_buffer.append(m_recvCtx.buf, m_recvCtx.buf + bytes);

    // 1) 아직 WS 아닌 상태라면, 핸드셰이크 시도
    if (!m_is_websocket) {
        if (try_handle_ws_handshake()) {
            // 핸드셰이크가 끝났고, 혹시 m_buffer에 남은 게 있으면 프레임 파싱
            if (!m_buffer.empty())
                handle_ws_data(m_buffer.data(), m_buffer.size());
            m_buffer.clear();
        }
        else {
            // WS가 아니면 (필요 시) 일반 TCP/라인 프로토콜로 처리
            // 여기서는 데모로 그냥 로그
            // __common.log_fmt(INFO, "[RECV FROM CLIENT](%s)", m_buffer.c_str());
            // m_buffer.clear();
            // ※ 실제로는 일반 TCP 모드 별도 파서를 두세요.
        }
    }
    else {
        // 2) 이미 WebSocket 모드라면 프레임 파싱
        handle_ws_data(m_buffer.data(), m_buffer.size());
        m_buffer.clear();
    }

    // 다음 수신 예약
    PostRecv(shared_from_this(), m_sock);
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
