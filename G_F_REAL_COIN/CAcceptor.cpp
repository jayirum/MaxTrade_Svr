#include "CAcceptor.h"
#include "CGlobals.h"

extern CGlobals gCommon;


CAcceptor::CAcceptor(CSenderList* pSenders, boost::asio::io_context& ioContext, int port)
    : m_ioContext(ioContext), 
      m_acceptor(ioContext, tcp::endpoint(tcp::v4(), port))
{
    m_pSenders = pSenders;
    m_thrdFlag.setThreadRun();
    m_thrdAccept = std::thread(&CAcceptor::threadFunc_accept, this);
    m_thrdFlag.setThreadReady();
}

CAcceptor::~CAcceptor()
{
    m_acceptor.cancel();
    m_acceptor.close();
    m_thrdFlag.setThreadStop();
    if (m_thrdAccept.joinable())
        m_thrdAccept.join();
}

void CAcceptor::threadFunc_accept()
{
    while (!m_thrdFlag.isReady())
    {
        if (!m_thrdFlag.isRunning()) return;
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }


    while (m_thrdFlag.isRunning() && m_thrdFlag.isReady())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));
        boost::system::error_code ec;
        auto clientSocket = std::make_shared<tcp::socket>(m_ioContext);
        m_acceptor.accept(*clientSocket, ec);
        if (ec) {
            gCommon.log(ERR, TRUE, "Accept error:", ec.message().c_str());
            continue;
        }
        unsigned int fd = (unsigned int)clientSocket->native_handle();

        gCommon.log(INFO, TRUE, "[fd:%d] Accept", fd);

        m_pSenders->addClient_to_oneSender(clientSocket);
    }
}
