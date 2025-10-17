#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "CSenderList.h"
#include "CGlobals.h"
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include  <chrono>
#include <sstream>
#include <iostream>

extern CGlobals		gCommon;

CSender::CSender(int idx) : m_my_idx(idx), m_dwThreadId(0)
{
    InitializeCriticalSectionAndSpinCount(&m_cs, 500);

    m_thrdFlag.set_idle();
    m_thrdSender = std::thread(&CSender::threadFunc_send_to_clients, this);
    m_dwThreadId = GetThreadId(m_thrdSender.native_handle());
    m_thrdFlag.set_run();
}

CSender::~CSender() 
{
    m_thrdFlag.set_stop();

    if (m_thrdSender.joinable()) {
        m_thrdSender.join();
        DeleteCriticalSection(&m_cs);
    }
}


void CSender::push_clientSock(std::shared_ptr<tcp::socket> boostsock, string ip)
{
    lock();
    unsigned int fd = (unsigned int)boostsock->native_handle();
    m_mapClientSocks[fd] = std::make_unique<TClientSock>(move(boostsock), ip);
    unlock();
    gCommon.debug("[push_clientSock]add to map(%d:%s)(size:%d)", fd, ip.c_str(), m_mapClientSocks.size());
}

// Worker 실행 함수
void CSender::threadFunc_send_to_clients()
{
    while (!m_thrdFlag.is_stopped())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        MSG PeekMsg;
        while (PeekMessage(&PeekMsg, NULL, 0, 0, PM_REMOVE) == TRUE)
        {
            if (PeekMsg.message != __MAX::WM_MARKET_DATA) {
                break;
            }

            __MAX::TData* pRecvData = reinterpret_cast<__MAX::TData*>(PeekMsg.lParam);

            //
            lock();
            send_to_clients(pRecvData);
            unlock();
            //

            gCommon.memPool.release(pRecvData);
        }
    }

    printf("void CSender::threadFunc_send_to_clients() 종료...\n");
}

void CSender::send_to_clients(__MAX::TData* pSendData)
{
    std::map<unsigned int, std::unique_ptr<TClientSock>>::iterator it;
    
    for (it=m_mapClientSocks.begin(); it!=m_mapClientSocks.end(); )
    {
        unsigned int fd = (*it).first;

        boost::system::error_code error;

        /*****/
        size_t bytesSent = boost::asio::write( *(it->second->sock), boost::asio::buffer(pSendData->d), error);
        /*****/
        if (error) 
        {
            if (error == boost::asio::error::broken_pipe ||
                error == boost::asio::error::connection_reset ||
                error == boost::asio::error::connection_aborted) 
            {
                gCommon.log(ERR, TRUE, "[DISCONNECT][IP:%-15.15s][SOCKET:%d][IDX:%d]", it->second->ip.c_str(), fd, m_my_idx);
            }
            else 
            {
                gCommon.log(ERR, TRUE, "[SEND ERROR][IP:%-15.15s][SOCKET:%d][IDX:%d](%s)", it->second->ip.c_str(), fd, m_my_idx, error.message().c_str());
            }
            it->second->sock->close();
            it = m_mapClientSocks.erase(it);
        }
        else 
        {
            ++it;
        }
    }
    
}
