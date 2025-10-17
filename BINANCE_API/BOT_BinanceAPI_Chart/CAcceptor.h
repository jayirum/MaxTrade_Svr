#pragma once

#include "CSenderList.h"
#include <iostream>
#include <boost/asio.hpp>
#include <atomic>
#include <thread>
#include "CGlobals.h"

using boost::asio::ip::tcp;

class CAcceptor
{
public:
    CAcceptor(CSenderList* pSenders, boost::asio::io_context& ioContext, int port);
    ~CAcceptor();
    void Initialize() { };
    void DeIitialize() { m_thrdFlag.setThreadStop(); }
private:
    void threadFunc_accept();

private:
    boost::asio::io_context&    m_ioContext;
    tcp::acceptor               m_acceptor;
    std::thread                 m_thrdAccept;
    CThreadFlag                 m_thrdFlag;
    CSenderList*                m_pSenders;
};