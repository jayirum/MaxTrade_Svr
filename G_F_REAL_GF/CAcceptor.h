#pragma once

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
    CAcceptor(boost::asio::io_context& ioContext, int port);
    ~CAcceptor();
    bool Initialize();
    void DeIitialize() { m_thrdFlag.set_stop(); }
private:
    void threadFunc_accept();
    void threadFunc_accept_Internal();

private:
    boost::asio::io_context&        m_ioContext;
    boost::optional<tcp::acceptor>  m_acceptor;
    std::thread                     m_thrdAccept;
    __MAX::CThreadFlag			    m_thrdFlag;
    int                             m_listenPort;
};