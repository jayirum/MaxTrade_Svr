#include "CSenderQList.h"
#include "../Common/AlphaInc.h"
#include "CGlobals.h"
extern CGlobals gCommon;


CSenderQ::CSenderQ()
{
    InitializeCriticalSectionAndSpinCount(&m_cs, 500);
}
CSenderQ::~CSenderQ()
{
    //DeleteCriticalSection(&m_cs);
}

void CSenderQ::Push(TMessage& msg)
{
    Lock();
    m_messageQ.push(msg);
    Unlock();
}

bool CSenderQ::Pop(TMessage& msg)
{
    Lock();
    bool ret = false;
    if (!m_messageQ.empty()) {
        msg = std::move(m_messageQ.front());
        m_messageQ.pop();
        ret = true;
    }
    Unlock();
    return ret;
}


CSenderQList::CSenderQList()
{
    InitializeCriticalSectionAndSpinCount(&m_cs,500);
}


CSenderQList::~CSenderQList()
{ 
    DeInitialize();
    if (m_thrdPushQ.joinable()) {
        m_thrdPushQ.join();
        DeleteCriticalSection(&m_cs);
    }
}

void CSenderQList::DeInitialize()
{
    if (m_thrdFlag.isRunning()) {

        m_thrdFlag.setThreadStop();
        PostThreadMessage(m_dwThreadId, WM_QUIT, (WPARAM)0, (LPARAM)0);

        Lock();
        for (int i = 0; i < m_vecSenderQs.size(); i++)
        {
            delete m_vecSenderQs[i];    // Destructor of CSendQ doesn't have special logics.
        }
        Unlock();
    }
}

bool CSenderQList::Initialize()
{
    Lock();
    for (int i = 0; i < gCommon.getSendersNum(); i++)
    {
        m_vecSenderQs.push_back(new CSenderQ());
    }
    Unlock();
    gCommon.log(INFO, FALSE, "[%d] Create Data Q", m_vecSenderQs.size());

    m_thrdFlag.setThreadRun();
    m_thrdPushQ = std::thread(&CSenderQList::Threadfunc_push_to_allQueues, this);
    m_dwThreadId = GetThreadId(m_thrdPushQ.native_handle());
    
    m_thrdFlag.setThreadReady();

    return true;
}

bool CSenderQList::deQueue_for_oneSender(int idx, TMessage& msg)
{
    return ((CSenderQ*)m_vecSenderQs[idx])->Pop(msg);
}

void CSenderQList::enQueue_to_allSenders(const std::string& data)
{
    TMessage* msg = new TMessage(data);
    PostThreadMessage(m_dwThreadId, WM_MARKET_DATA, (WPARAM)0, (LPARAM)msg);
}

void CSenderQList::Threadfunc_push_to_allQueues()
{
    while (!m_thrdFlag.isReady())
    {
        if (!m_thrdFlag.isRunning()) return;
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    while (m_thrdFlag.isRunning() && m_thrdFlag.isReady())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        MSG PeekMsg;
        while (PeekMessage(&PeekMsg, NULL, 0, 0, PM_REMOVE) == TRUE)
        {
            if (PeekMsg.message == WM_QUIT)
                return;

            if (PeekMsg.message == WM_MARKET_DATA)
            {
                TMessage rcvMsg(((TMessage*)PeekMsg.lParam)->data_s());

                /*****/
                push_to_allQueues(rcvMsg);
                /*****/
            }
            delete (TMessage*)PeekMsg.lParam;  // 원본 메시지 해제
        }
    }
}


void CSenderQList::push_to_allQueues(TMessage& msg)
{
    for (int i = 0; i < m_vecSenderQs.size(); i++)
    {
        if ( !m_thrdFlag.isRunning() || !m_thrdFlag.isReady())
            return;

        ((CSenderQ*)m_vecSenderQs[i])->Push(msg);
    }
}