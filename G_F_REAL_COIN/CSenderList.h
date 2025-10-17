#pragma once
#pragma warning(disable:4473)

#include <boost/asio/ip/tcp.hpp>

#include <windows.h>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <algorithm>

#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include "CSenderQList.h"
#include "CGlobals.h"

using tcp = boost::asio::ip::tcp;

class CSender
{
public:
    CSender(int idx);
    ~CSender();

    unsigned int getThreadID() { return m_dwThreadId; }
    void push_clientSock(std::shared_ptr<tcp::socket>& boostsock);
private:
    void threadFunc_send_to_allMyClients();
    void Lock() { EnterCriticalSection(&m_cs); }
    void Unlock() { LeaveCriticalSection(&m_cs); }
    void send_to_allMyClients(TMessage& msg);

 
private:
    int                 m_MyIdx;
    CThreadFlag         m_thrdFlag;
    std::thread         m_thrdSender;
    DWORD               m_dwThreadId;

    std::map<unsigned int, std::shared_ptr<tcp::socket>> m_mapClientSocks;
    CRITICAL_SECTION    m_cs;
};


class CSenderList
{
public:
    CSenderList();
    ~CSenderList();

    bool Initialize();
    void DeInitialize();
    void addClient_to_oneSender(std::shared_ptr<tcp::socket>& boostsock);

private:
    void create_senders(int num);
    
private:
    std::vector<CSender*>   m_vecSenders;
};

