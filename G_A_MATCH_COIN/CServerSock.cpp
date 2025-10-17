#include "CServerSock.h"
#include "CGlobals.h"
#include "../Common/TimeUtils.h"

extern CGlobals gCommon;

CServerSock::CServerSock(boost::asio::io_context& iocAccept, COrderProcess* orderProcess)
    //m_acceptor(iocAccept, tcp::endpoint(tcp::v4(), port))
    : m_guardReturn(boost::asio::make_work_guard(m_iocReturn))
    , m_orderProcess(orderProcess)
{
    m_acceptor.emplace(iocAccept);
    m_thrdFlag.set_idle();
    m_thrdParsing = std::thread(&CServerSock::threadfunc_parse_deploy, this);
    m_thrdReturnQ = std::thread(&CServerSock::threadfunc_return_to_client, this);
    m_thrdReturn = std::thread([this] {m_iocReturn.run(); });
}


bool CServerSock::Initialize(int port)
{
    try {
        m_acceptor->open(tcp::v4());
        m_acceptor->set_option(boost::asio::socket_base::reuse_address(true));
        m_acceptor->bind(tcp::endpoint(tcp::v4(), port));
        m_acceptor->listen();

        gCommon.log(INFO, TRUE, "[Server Port Binding 성공](%d)", port);
    }
    catch (const boost::system::system_error& e) {
        gCommon.log(ERR, TRUE, "[Server socket 초기화 오류!!!](Port:%d)(%s)", port, e.what());
        return false;
    }

    m_thrdFlag.set_run();
    Accept();
    return true;
}
CServerSock::~CServerSock()
{
    m_thrdFlag.set_stop();

    m_guardReturn.reset();
    m_iocReturn.stop();
    
    if (m_thrdParsing.joinable()) {
        m_thrdParsing.join();
    }
    if (m_thrdReturn.joinable()) {
        m_thrdReturn.join();
    }
    if (m_thrdReturnQ.joinable()) {
        m_thrdReturnQ.join();
    }
}

void CServerSock::Accept()
{
    auto sockClient = std::make_shared<tcp::socket>(m_acceptor->get_executor());
    m_acceptor->async_accept(
        *sockClient,
        [this, sockClient](boost::system::error_code ec) 
        {
            if (!ec) {
                gCommon.log(INFO, TRUE, "Accept Client Socket[fd:%d]", (unsigned int)sockClient->native_handle());
                recv_from_client(sockClient);
            }
            Accept();  // 계속해서 새로운 연결을 받음
        });
}

void CServerSock::recv_from_client(std::shared_ptr<tcp::socket> sockClient)
{
    auto arrBuff = std::make_shared<std::array<char, 1024>>();
    sockClient->async_read_some(
        boost::asio::buffer(*arrBuff),
        [this, sockClient, arrBuff](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                //gCommon.log(INFO, FALSE, "[BIZ->MATCH](%s)\n", arrBuff.get()->data());
                
                string id;
                std::string receivedData(arrBuff->data(), length);
                __MAX::TPC001* packet = (__MAX::TPC001*)receivedData.c_str();
                if (receivedData.find("PC001") != string::npos)
                {
                    //@G_B_BIZ, @G_B_BL
                    id = __UTILS::trim(std::string(packet->header.userid, sizeof(packet->header.userid)));

                    std::lock_guard<std::mutex> lock(m_mutexClients);
                    m_mapClients[id] = sockClient;

                    gCommon.debug("[PC001수신](%s)", id.c_str());
                }
                else
                {
                    gCommon.debug("[RECV from Client](%s)", receivedData.c_str());
                    std::lock_guard<std::mutex> lock(m_mutexParsingBuffer);
                    m_parsingBuffer += receivedData;
                }
                recv_from_client(sockClient);  // 계속 읽기
            }
        }
    );
}


void CServerSock::threadfunc_parse_deploy()
{
    while (!m_thrdFlag.is_stopped())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if (!m_parsingBuffer.empty())
        {
            std::lock_guard<std::mutex> lock(m_mutexParsingBuffer);
            
            size_t pos;
            size_t len = m_parsingBuffer.size();
            gCommon.debug("[PARSING-1](%s)", m_parsingBuffer.c_str());
            while ((pos = m_parsingBuffer.find("\n")) != std::string::npos)
            {
                std::string packet = m_parsingBuffer.substr(0, pos);  // 첫 번째 패킷 추출
                gCommon.debug("[PARSING-2](%s)", packet.c_str());
                m_parsingBuffer.erase(0, pos + 1);  // 처리된 부분 삭제 (2는 "\r\n" 길이)

                size_t len2 = m_parsingBuffer.size();
                gCommon.log(INFO, TRUE, "[BIZ->MATCH](%s)", packet.c_str());
                
                m_orderProcess->AcceptOrder(packet);
            }
        }
    }
}


void CServerSock::threadfunc_return_to_client()
{
    while (!m_thrdFlag.is_stopped())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        __MAX::TData* pData = gCommon.returnQ.pop();
        if (pData == nullptr)
            continue;

        gCommon.debug("[리턴패킷POP](%s)", pData->d);
        boost::asio::post(m_iocReturn, [this, pData]() {processReturnData(pData); });
    }

}

void CServerSock::processReturnData(__MAX::TData* pInData)
{
    gCommon.debug("[processReturnData](%s)", pInData->d);
    std::lock_guard<std::mutex> lock2(m_mutexClients);
    auto it = m_mapClients.find("@G_B_BIZ");
    if (it == m_mapClients.end())
    {
        gCommon.log(ERR, TRUE, "[CServerSock::processReturnData]데이터 전송을 위한 (@G_B_BIZ) 의 소켓이 저장되어 있지 않다.");
        return;
    }

    char sendBuff[1024] = { 0 };
    strcpy(sendBuff, pInData->d);
    size_t sendLen = sizeof(__MAX::TTA001);

    gCommon.memPool.release(pInData);

    /*****/
    auto itSock = it->second;
    boost::asio::async_write(*itSock, boost::asio::buffer(sendBuff, sendLen),
        [sendBuff, itSock](boost::system::error_code ec, std::size_t) {
            if (ec) {
                gCommon.log(ERR, TRUE, "[CServerSock::processReturnData]전송에러(%s)", ec.message().c_str());
            }
            else {
                gCommon.log(INFO, FALSE, "[MATCH->BIZ](%s)", sendBuff);
            }
        }
    );
}
