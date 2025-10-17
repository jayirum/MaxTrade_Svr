/*
    Queue for relaying data between [CBinanceApi] and [CSenderList]

    
    CSenderQ
     - multiple numbers : as many as CSenders
     - each CSenderQ has its own std::queue 

    CSenderQList
     - vector<CSenderQ>
     - thread for pushing date to all CSenderQ
*/

#pragma once

#include "../../Common/MaxTradeInc.h"
#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
//#include <queue>
#include <windows.h>
#include "CGlobals.h"
#include <queue>
#include <mutex>

using namespace std;

constexpr size_t MSG_SIZE = 512;
//constexpr size_t Q_SIZE = 1000;


class TMessage
{
public:
    TMessage() :m_len(0)
    {}
    TMessage(const std::string& str) :m_len(0)
    {
        m_len = (int)__min(str.size(), sizeof(m_data) - 1);
        ZeroMemory(m_data, sizeof(m_data));
        memcpy(m_data, str.c_str(), m_len);
    }

    ~TMessage() {};

    
    char*       data() { return m_data; }
    std::string data_s() { return std::string(m_data); }
    int         size() { return m_len; }
    
private:
    char    m_data[MSG_SIZE];
    int     m_len{ 0 };
};



class CSenderQ
{
public:
    CSenderQ();
    ~CSenderQ();

    void Push(TMessage& msg);
    bool Pop(TMessage& msg);

private:
    std::queue < TMessage>  m_messageQ;
    std::mutex              m_mxMsgQ;
};

class CSenderQList
{
public:
    CSenderQList();
    ~CSenderQList();

    bool Initialize();
    void enQueue_to_allSenders(char* pzPacket);
    bool deQueue_for_oneSender(int idx, TMessage& msg);
private:
    void DeInitialize();
    void Threadfunc_push_to_allQueues();

    void push_to_allQueues(TMessage& msg);

private:
    std::vector<CSenderQ *>   m_vecSenderQs;
    std::thread             m_thrdPushQ;
    unsigned int            m_dwThreadId;

    __MAX::CThreadFlag		m_thrdFlag;
    std::mutex              m_mxSenderQ;
};

