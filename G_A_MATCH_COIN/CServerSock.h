#pragma once

#include "../Common/MaxTradeInc.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <atomic>
#include <thread>
#include <deque>
#include "CGlobals.h"
#include "COrderProcess.h"

using boost::asio::ip::tcp;
using namespace std;

class CServerSock
{
public:
    CServerSock(boost::asio::io_context& iocAccept, COrderProcess* orderProcess);
    ~CServerSock();
    
    bool Initialize(int port);

private:
    
    void Accept();
    void recv_from_client(std::shared_ptr<tcp::socket> sockClient);
    void threadfunc_parse_deploy();
    void threadfunc_return_to_client();
    void processReturnData(__MAX::TData* pData);
private:
    boost::optional<tcp::acceptor>               m_acceptor;

    boost::asio::io_context     m_iocReturn;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_guardReturn;  // io_context 실행 유지
    std::thread                 m_thrdReturn;

    map<string, std::shared_ptr<tcp::socket>>   m_mapClients;
    std::mutex                                  m_mutexClients;

    std::thread     m_thrdReturnQ;

    //
    std::mutex      m_mutexParsingBuffer;
    std::string     m_parsingBuffer;
    std::thread     m_thrdParsing;
    //


    __MAX::CThreadFlag		m_thrdFlag;

    std::mutex          m_mutexTest;
    deque<string>       m_deqTest;

    COrderProcess* m_orderProcess;
};