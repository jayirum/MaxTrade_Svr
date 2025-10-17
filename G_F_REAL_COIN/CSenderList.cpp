#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "CSenderList.h"
#include "CGlobals.h"
#include <Windows.h>
#include "../Common/AlphaInc.h"
#include "../Common/Util.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include  <chrono>
#include <sstream>
#include "main.h"
#include "CSenderQList.h"
#include <iostream>

extern CGlobals		gCommon;
extern CSenderQList	gQList;

/// <summary>
/// 
/// </summary>
/// <param name="id"></param>
CSender::CSender(int idx) : m_MyIdx(idx), m_dwThreadId(0)
{
    InitializeCriticalSectionAndSpinCount(&m_cs, 500);

    m_thrdFlag.setThreadRun();
    m_thrdSender = std::thread(&CSender::threadFunc_send_to_allMyClients, this);
    m_dwThreadId = GetThreadId(m_thrdSender.native_handle());
    m_thrdFlag.setThreadReady();
}

CSender::~CSender() 
{
    m_thrdFlag.setThreadStop();

    if (m_thrdSender.joinable()) {
        m_thrdSender.join();
        DeleteCriticalSection(&m_cs);
    }
}


void CSender::push_clientSock(std::shared_ptr<tcp::socket>& boostsock)
{
    Lock();
    unsigned int fd = (unsigned int)boostsock->native_handle();
    m_mapClientSocks[fd] = boostsock;
    Unlock();
}

// Worker 실행 함수
void CSender::threadFunc_send_to_allMyClients()
{
    while (!m_thrdFlag.isReady())
    {
        if (!m_thrdFlag.isRunning()) return;
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }


    //char zPath[256];
    //sprintf(zPath, "./logWorker_[%d].log", m_MyIdx);
    //std::ofstream outFile(zPath, std::ios::app);

    //if (!outFile.is_open()) 
    //    return;

    while (m_thrdFlag.isRunning() && m_thrdFlag.isReady())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        TMessage msg;
        bool exist = gQList.deQueue_for_oneSender(m_MyIdx, msg);
        if (exist == false)
        {
            continue;
        }

            // Get current time
 ///*           auto now = std::chrono::system_clock::now();
 //           auto timeT = std::chrono::system_clock::to_time_t(now);
 //           std::tm localTime;
 //           localtime_s(&localTime, &timeT);

 //           std::ostringstream timeStream;
 //           timeStream << std::put_time(&localTime, "[%Y-%m-%d %H:%M:%S] ");*/

 //           // Log with timestamp
 //           outFile << "[" << timeStream.str() << "]" << msg.data() << std::endl;
 //           outFile.flush();  // Flush to ensure real-time logging
        
        /*****/
        send_to_allMyClients(msg);
        /*****/
    }

    printf("void CSender::threadFunc_send_to_allMyClients() 종료...\n");
}

void CSender::send_to_allMyClients(TMessage& msg)
{
    std::map<unsigned int, std::shared_ptr<tcp::socket>>::iterator it;
    Lock();
    for (it=m_mapClientSocks.begin(); it!=m_mapClientSocks.end(); )
    {
        unsigned int fd = (*it).first;

        boost::system::error_code error;

        /*****/
        size_t bytesSent = boost::asio::write( *(*it).second, boost::asio::buffer( msg.data_s()), error);
        /*****/
        if (error) 
        {
            if (error == boost::asio::error::broken_pipe ||
                error == boost::asio::error::connection_reset ||
                error == boost::asio::error::connection_aborted) 
            {
                gCommon.log(ERR, TRUE, "Client socket has been closed(%d)", fd);
            }
            else 
            {
                gCommon.log(ERR, TRUE, "[%d]send error(fd:%d)(%s)", m_MyIdx, fd, error.message().c_str());
            }
            (*it).second->close();
            it = m_mapClientSocks.erase(it);
        }
        else 
        {
            ++it;
            //gCommon.log(INFO, TRUE, "[%d]sent(fd:%d)(%s)", m_MyIdx, fd, pMsg->data());
        }
    }
    Unlock();
}


/////////////////////////////////////////////////////////////////////////////
/// <summary>
/// 
/// </summary>
/// /////////////////////////////////////////////////////////////////////////
CSenderList::CSenderList()
{
}

CSenderList::~CSenderList()
{
    DeInitialize();
}

void CSenderList::DeInitialize()
{
    //for (int i=0; i<m_vecSenders.size(); i++)
    //    delete m_vecSenders[i];
}

void CSenderList::create_senders(int num)
{
    for (int i = 0; i < num; i++) {
        m_vecSenders.push_back(new CSender(i));
    }

    gCommon.log(INFO, TRUE, "Create [%d] Senders", num);
}

bool CSenderList::Initialize()
{
    create_senders(gCommon.getSendersNum());
    return true;
}

void CSenderList::addClient_to_oneSender(std::shared_ptr<tcp::socket>& boostsock)
{
    int idx = gCommon.gClientNum.IncNum();
    m_vecSenders[idx]->push_clientSock(boostsock);
}

