#include "CAcceptor.h"
#include "CGlobals.h"

extern CGlobals gCommon;


CAcceptor::CAcceptor(CSenderList* pSenders, boost::asio::io_context& ioContext, int port)
    : m_ioContext(ioContext), m_listenPort(port)
{
    m_acceptor.emplace(m_ioContext);

    m_pSenders = pSenders;
    m_thrdFlag.set_idle();
    m_thrdAccept = std::thread(&CAcceptor::threadFunc_accept, this);

   
}

CAcceptor::~CAcceptor()
{
    m_acceptor->cancel();
    m_thrdFlag.set_stop();
    if (m_thrdAccept.joinable())
        m_thrdAccept.join();

    m_acceptor->close();
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

        if (!m_thrdFlag.is_running()) {
            continue;
        }
        boost::system::error_code ec;
        auto clientSocket = std::make_shared<tcp::socket>(m_acceptor->get_executor());
        //auto clientSocket = std::make_shared<tcp::socket>(m_ioContext);

        m_acceptor->accept(*clientSocket, ec);
        if (ec) {
            gCommon.log(ERR, TRUE, "[CAcceptor::threadFunc_accept]Accept error(%d)(%s)", ec.value(), ec.message().c_str());
            continue;
        }
        unsigned int fd = (unsigned int)clientSocket->native_handle();

        gCommon.log(INFO, TRUE, "Accept...[fd:%d]", fd);

        m_pSenders->addClient_to_oneSender(clientSocket);
    }
}
