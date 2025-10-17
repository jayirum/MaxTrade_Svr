#include "CSenderQList.h"
#include "../../Common/AlphaInc.h"
#include "CGlobals.h"
extern CGlobals gCommon;


CSenderQ::CSenderQ()
{
    //InitializeCriticalSectionAndSpinCount(&m_cs, 500);
}
CSenderQ::~CSenderQ()
{
    //DeleteCriticalSection(&m_cs);
}

void CSenderQ::Push(TMessage& msg)
{
    //Lock();
    std::lock_guard <mutex> lock(m_mxMsgQ);
    m_messageQ.push(msg);
   // Unlock();
}

bool CSenderQ::Pop(TMessage& msg)
{
    //Lock();
    std::lock_guard <mutex> lock(m_mxMsgQ);
    bool ret = false;
    if (!m_messageQ.empty()) {
        msg = std::move(m_messageQ.front());
        m_messageQ.pop();
        ret = true;
    }
    //Unlock();
    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////////////////////

CSenderQList::CSenderQList() 
{
    //InitializeCriticalSectionAndSpinCount(&m_cs,500);
}


CSenderQList::~CSenderQList()
{ 
    DeInitialize();
}

void CSenderQList::DeInitialize()
{
    if (m_thrdFlag.isRunning())
    {
        m_thrdFlag.setThreadStop();
        PostThreadMessage(m_dwThreadId, WM_QUIT, (WPARAM)0, (LPARAM)0);

        if (m_thrdPushQ.joinable())
            m_thrdPushQ.join();

        //Lock();
        std::lock_guard <mutex> lock(m_mxSenderQ);
        for (int i = 0; i < m_vecSenderQs.size(); i++)
        {
            delete m_vecSenderQs[i];    // Destructor of CSendQ doesn't have special logics.
        }
        //Unlock();
        //DeleteCriticalSection(&m_cs);
    }
}

bool CSenderQList::Initialize()
{
    //Lock();
    std::lock_guard <mutex> lock(m_mxSenderQ);
    for (int i = 0; i < gCommon.getSendersNum(); i++)
    {
        m_vecSenderQs.push_back(new CSenderQ());
    }
    //Unlock();
    gCommon.log(INFO, FALSE, "[%d] 의 Data Q 생성 성공", m_vecSenderQs.size());

    m_thrdFlag.setThreadRun();
    m_thrdPushQ = std::thread(&CSenderQList::Threadfunc_push_to_allQueues, this);
    m_dwThreadId = GetThreadId(m_thrdPushQ.native_handle());
    
    m_thrdFlag.setThreadReady();

    return true;
}

bool CSenderQList::deQueue_for_oneSender(int idx, TMessage& msg)
{
    return m_vecSenderQs[idx]->Pop(msg);
}

void CSenderQList::enQueue_to_allSenders(char* pzPacket)
{
    TMessage* msg = new TMessage(pzPacket);
    PostThreadMessage(m_dwThreadId, WM_MARKET_DATA, (WPARAM)0, (LPARAM)msg);
}

void CSenderQList::Threadfunc_push_to_allQueues()
{
    while (!m_thrdFlag.isReady())
    {
        if (!m_thrdFlag.isRunning()) return;
        std::this_thread::yield();
    }


    while (m_thrdFlag.isRunning() && m_thrdFlag.isReady())
    {
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
        if (!m_thrdFlag.isRunning() || !m_thrdFlag.isReady())
            return;

        ((CSenderQ*)m_vecSenderQs[i])->Push(msg); 
    }
}