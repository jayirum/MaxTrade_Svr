#pragma once
#pragma warning(disable:4473)

#include <boost/asio/ip/tcp.hpp>
#include "../Common/MaxTradeInc.h"
#include "CGlobals.h"
#include <windows.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <algorithm>

#include <iostream>
#include <thread>
#include <vector>
#include <map>

extern CGlobals		gCommon;

using tcp = boost::asio::ip::tcp;

struct TClientSock
{
    std::shared_ptr<tcp::socket> sock;
    string ip;

    TClientSock(std::shared_ptr<tcp::socket> s, string i) :sock(std::move(s)), ip(i) {}
}; 

class CSender
{
public:
    CSender(int idx);
    ~CSender();

    unsigned int getThreadID() { return m_dwThreadId; }
    void push_clientSock(std::shared_ptr<tcp::socket> boostsock, string ip);
private:
    void threadFunc_send_to_clients();
    void threadFunc_send_to_clients_Internal();
    void send_to_clients(__MAX::TData* pData);

    void lock() { EnterCriticalSection(&m_cs); }
    void unlock() { LeaveCriticalSection(&m_cs); }
    
 
private:
    int                 m_my_idx;
    std::thread         m_thrdSender;
    DWORD               m_dwThreadId;
    __MAX::CThreadFlag  m_thrdFlag;

    std::map<unsigned int, std::unique_ptr<TClientSock>> m_mapClientSocks;
    CRITICAL_SECTION    m_cs;
};


class CSenderList
{
public:
    CSenderList() {};
    ~CSenderList() { DeInitialize(); };

    bool Initialize(int max_senders_cnt) {
        m_senders_max = max_senders_cnt;
        gCommon.debug( "m_senders_max:%d", m_senders_max);
        for (int i = 0; i < m_senders_max; i++) {
            m_vecSenders.push_back(std::make_unique<CSender>(i));
        }
        return (m_vecSenders.size()>0);
    }
    void DeInitialize() { m_vecSenders.clear(); };
    void deploy_client_sock(std::shared_ptr<tcp::socket> boostsock, string cientIp)
    {
        if(m_senders_max==0){
            gCommon.log(ERR, TRUE, "m_senders_max is zero! Cannot divide.");
            return;
        }

        int idx = m_curr_client_cnt++ % m_senders_max;
        gCommon.debug("[deploy_client_sock]idx:%d", idx);
        m_vecSenders[idx]->push_clientSock(boostsock, cientIp);
    }

    void sendData_all_clients(string& send_buff)
    {
        for (const auto& it : m_vecSenders) {
            __MAX::TData* p = gCommon.memPool.Alloc();
            memcpy(p->d, send_buff.c_str(), send_buff.size());

            PostThreadMessage(it->getThreadID(), __MAX::WM_MARKET_DATA, 0, (LPARAM)p);
        }
    }
    
private:
    std::vector < std::unique_ptr<CSender>>     m_vecSenders;
    int                                         m_senders_max;
    std::atomic<unsigned int>	                m_curr_client_cnt;
};
