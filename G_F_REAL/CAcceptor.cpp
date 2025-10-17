#include "CAcceptor.h"
#include "CGlobals.h"

extern CGlobals gCommon;
extern CSenderList gSenders;

CAcceptor::CAcceptor(boost::asio::io_context& ioContext, int port)
    : m_ioContext(ioContext), m_listenPort(port)
{
    m_acceptor.emplace(m_ioContext);

    m_thrdFlag.set_idle();
    m_thrdAccept = std::thread(&CAcceptor::threadFunc_accept, this);
}

CAcceptor::~CAcceptor()
{
    m_acceptor->cancel();
    m_acceptor->close();
    m_thrdFlag.set_stop();
    if (m_thrdAccept.joinable())
        m_thrdAccept.join();
}


bool CAcceptor::Initialize()
{
    try {
        m_acceptor->open(tcp::v4());
        m_acceptor->set_option(boost::asio::socket_base::reuse_address(true));
        m_acceptor->bind(tcp::endpoint(tcp::v4(), m_listenPort));
        m_acceptor->listen();

        gCommon.log(INFO, TRUE, "[Server Port Binding 성공](%d)", m_listenPort);
    }
    catch (const boost::system::system_error& e) {
        gCommon.log(ERR, TRUE, "[Server socket 초기화 오류!!!](Port:%d)(%s)", m_listenPort, e.what());
        return false;
    }

    m_thrdFlag.set_run();
    return true;
}


void CAcceptor::threadFunc_accept()
{
    while (!m_thrdFlag.is_stopped())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        boost::system::error_code ec;
        auto clientSocket = std::make_shared<tcp::socket>(m_acceptor->get_executor());
        m_acceptor->accept(*clientSocket, ec);
        if (ec) {
            gCommon.log(ERR, TRUE, "Accept error:", ec.message().c_str());
            continue;
        }
        unsigned int fd = (unsigned int)clientSocket->native_handle();

        // 클라이언트 IP 정보 가져오기
        std::string client_ip;
        boost::asio::ip::tcp::endpoint remote_ep = clientSocket->remote_endpoint(ec);
        if (!ec) {
            client_ip = remote_ep.address().to_string();
            unsigned short client_port = remote_ep.port();
        }

        gCommon.log(INFO, TRUE, "[CONNECT][IP:%-15.15s][SOCKET:%d]", client_ip.c_str(), fd);

        gSenders.deploy_client_sock(clientSocket, client_ip);
    }
}
