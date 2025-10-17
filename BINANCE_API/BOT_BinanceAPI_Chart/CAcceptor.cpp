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

    gCommon.log(INFO, TRUE, "Acceptor listen port(%d)", port);
}

CAcceptor::~CAcceptor()
{
    m_acceptor.cancel();
    m_thrdFlag.setThreadStop();
    if (m_thrdAccept.joinable())
        m_thrdAccept.join();

    m_acceptor.close();
}

void CAcceptor::threadFunc_accept()
{
    while (!m_thrdFlag.isReady())
    {
        if (!m_thrdFlag.isRunning()) return;
        std::this_thread::yield();
    }


    while (m_thrdFlag.isRunning() && m_thrdFlag.isReady())
    {
        boost::system::error_code ec;
        auto clientSocket = std::make_shared<tcp::socket>(m_ioContext);

        m_acceptor.accept(*clientSocket, ec);
        if (ec) {
            gCommon.log(ERR, TRUE, "[CAcceptor::threadFunc_accept]Accept error(%d)(%s)", ec.value(), ec.message().c_str());
            continue;
        }
        unsigned int fd = (unsigned int)clientSocket->native_handle();

        gCommon.log(INFO, TRUE, "Accept...[fd:%d]", fd);

        m_pSenders->addClient_to_oneSender(clientSocket);
    }
}
