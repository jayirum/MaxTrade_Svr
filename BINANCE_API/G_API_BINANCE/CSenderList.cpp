#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include "CSenderList.h"
#include "CGlobals.h"
#include <Windows.h>
#include "../../Common/AlphaInc.h"
#include "../../Common/Util.h"
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
    m_thrdFlag.set_idle();
    m_thrdSender = std::thread(&CSender::threadFunc_send_to_allMyClients, this);
    m_dwThreadId = GetThreadId(m_thrdSender.native_handle());
}

CSender::~CSender() 
{
    m_thrdFlag.set_stop();
    std::map<unsigned int, std::shared_ptr<tcp::socket>>::iterator it;
    for (it = m_mapClientSocks.begin(); it != m_mapClientSocks.end(); )
    {
        it->second->cancel();
        it->second->close();
    }

    if (m_thrdSender.joinable()) {
        m_thrdSender.join();
    }
}


void CSender::push_clientSock(std::shared_ptr<tcp::socket>& boostsock)
{
    std::lock_guard<mutex> lock(m_mxClients);
    unsigned int fd = (unsigned int)boostsock->native_handle();
    m_mapClientSocks[fd] = boostsock;
}

// Worker 실행 함수
void CSender::threadFunc_send_to_allMyClients()
{
    while (!m_thrdFlag.is_stopped())
    {
        std::this_thread::sleep_for(chrono::microseconds(1000));
        
        TMessage msg;
        bool bExists = gQList.deQueue_for_oneSender(m_MyIdx, msg);
        if (bExists == false)
        {
            continue;
        }

        /*****/
        send_to_allMyClients(msg);
        /*****/

    }

    printf("void CSender::threadFunc_send_to_allMyClients() 종료...\n");
}

void CSender::send_to_allMyClients(TMessage& msg)
{
    std::map<unsigned int, std::shared_ptr<tcp::socket>>::iterator it;
    //Lock();
    std::lock_guard<mutex> lock(m_mxClients);
    for (it=m_mapClientSocks.begin(); it!=m_mapClientSocks.end(); )
    {
        if (m_thrdFlag.is_stopped())
            return;

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
                gCommon.log(ERR, TRUE, "[CSender::send_to_allMyClients] Client socket has been closed(%d)", fd);
            }
            else 
            {
                gCommon.log(ERR, TRUE, "[CSender::send_to_allMyClients-%d]send error(fd:%d)(%d)(%s)", m_MyIdx, fd, error.value(), error.message().c_str());
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
    //Unlock();
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

    gCommon.log(INFO, TRUE, "Create [%d] workers", num);
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

