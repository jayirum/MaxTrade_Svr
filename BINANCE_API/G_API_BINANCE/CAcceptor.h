#pragma once

#include "../../Common/maxtradeinc.h"
#include "CSenderList.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <atomic>
#include <thread>
#include "CGlobals.h"

using boost::asio::ip::tcp;

class CAcceptor
{
public:
    CAcceptor(CSenderList* pSenders, boost::asio::io_context& ioContext, int port);
    ~CAcceptor();
    bool Initialize();
    void DeIitialize() { m_thrdFlag.set_stop(); }
private:
    void threadFunc_accept();

private:
    boost::asio::io_context&        m_ioContext;
    boost::optional<tcp::acceptor>  m_acceptor;
    std::thread                     m_thrdAccept;
    __MAX::CThreadFlag              m_thrdFlag;
    CSenderList*                    m_pSenders;
    int                             m_listenPort;
};