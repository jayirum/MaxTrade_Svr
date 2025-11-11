#pragma once

#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <stdexcept>
#include <algorithm>

using namespace std;

// 패킷 끝에 Enter (\r\n) 가 있다. CRLF

namespace ns_parser
{

constexpr  size_t  MAX_SIZE = 4096*10;

class CParser {
public:
    explicit CParser(){};

    // 바이트를 누적 (raw bytes; 인코딩은 가정하지 않음)
    bool append(const char* data, int len) 
    {
        if (len == 0){
            strcpy(m_msg, "length 를 입력하시오");
            return false ;
        }

        std::lock_guard<mutex> lock(m_mtx);

        // 과도한 증가 방지 (DoS 방어)
        if (m_buff.size() + len > MAX_SIZE) {
            sprintf(m_msg, "buffer overflow.(buffer size:%d > max 설정:%d", m_buff.size() + len, MAX_SIZE);
            return false;
        }
        m_buff.append(data, len);
        return  true;
    }

    // 완성된 '한 개의' 패킷(개행으로 구분) 꺼내기. 없으면 nullopt.
    // 반환은 개행을 제거한 payload (CRLF -> CR 제거됨).
    void get_one_packet(_Out_ string& rslt_packet, _Out_ int& len) 
    {
        len = 0;

        std::lock_guard<mutex> lock(m_mtx);

        // \n 기준 탐색
        auto pos = m_buff.find('\n');
        if (pos == std::string::npos) 
        {
            // 아직 한 줄이 완성되지 않았음 (너무 길어지면 방어)
            if (m_buff.size() > MAX_SIZE) {
                sprintf(m_msg, "패킷에 엔터가 없으면서 max size(%d) 보다 크다(size:%d). 버퍼 클리어", MAX_SIZE, m_buff.size());
            }
            return;
        }

        // 한 줄 추출 [0, pos) : \n 이전까지
        rslt_packet = m_buff.substr(0, pos);

        // CRLF 대응: 끝이 \r이면 제거
        if (!rslt_packet.empty() && rslt_packet.back() == '\r') {
            rslt_packet.pop_back();
        }

        // 소비: pos까지 + '\n' 1바이트 제거
        m_buff.erase(0, pos + 1);
        len = rslt_packet.size();
    }

    // 버퍼에 남은 미완성 데이터 길이
    size_t remain_size() const noexcept { return m_buff.size(); }

    // 남은 미완성 데이터 강제 취소/초기화
    void clear() { m_buff.clear(); }

    char*   get_msg(){ return m_msg;}
private:
    string      m_buff;
    char        m_msg[1024];
    std::mutex  m_mtx;
};

}