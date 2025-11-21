#pragma once

#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <optional>
#include <stdexcept>
#include <algorithm>
#include "CGlobals.h"

using namespace std;

// 패킷 끝에 Enter (\r\n) 가 있다. CRLF

namespace ns_parser
{

constexpr  size_t  MAX_SIZE = 4096*1000;  //4M

class CParser {
public:
    explicit CParser(){
        _thrd_parsing = std::thread(&CParser::thrd_parsing, this);
    };
    ~CParser() {
        _is_continue = false;
        if(_thrd_parsing.joinable()) _thrd_parsing.join();
    }


    // 바이트를 누적 (raw bytes; 인코딩은 가정하지 않음)
    bool append(const char* data, int len) 
    {
        if (len == 0){
            strcpy(_msg, "length 를 입력하시오");
            return false ;
        }

        std::lock_guard<mutex> lock(_mtxRecv);

        // 과도한 증가 방지 (DoS 방어)
        if (m_buff.size() + len > MAX_SIZE) {
            sprintf(_msg, "buffer overflow.(buffer size:%d > max 설정:%d", m_buff.size() + len, MAX_SIZE);
            return false;
        }
        m_buff.append(data, len);
        return  true;
    }


    void thrd_parsing()
    {
        while(_is_continue)
        {
            Sleep(1);
            string one_packet;

            { // for lock
                std::lock_guard<mutex> lock(_mtxRecv);
                if( m_buff.empty() ) continue;

                // \n 기준 탐색
                auto pos = m_buff.find('\n');
                if (pos == std::string::npos)
                {
                    // 아직 한 줄이 완성되지 않았음 (너무 길어지면 방어)
                    if (m_buff.size() > MAX_SIZE) {
                        sprintf(_msg, "패킷에 엔터가 없으면서 max size(%d) 보다 크다(size:%d). 버퍼 클리어", MAX_SIZE, m_buff.size());
                        __common.debug_fmt("[1](%s)", _msg);
                    }
                    continue;
                }

                // 한 줄 추출 [0, pos) : \n 이전까지
                one_packet = m_buff.substr(0, pos);

                // 소비: pos까지 + '\n' 1바이트 제거
                m_buff.erase(0, pos + 1);
            }


            // CRLF 대응: 끝이 \r이면 제거
            if (!one_packet.empty() && one_packet.back() == '\r') {
                one_packet.pop_back();
            }
            std::lock_guard<mutex> lock(m_mtxRslt);
            m_rslt.push_back(one_packet);
        }
    }

    string get_one_packet(int& len)
    {
        len = 0;
        string packet;
        std::lock_guard<mutex> lock(m_mtxRslt);
        if( m_rslt.empty() ) return packet;
        packet = m_rslt.front();
        m_rslt.pop_front();
        len = packet.size();
        return packet;
    }

    // 버퍼에 남은 미완성 데이터 길이
    size_t remain_size() const noexcept { return m_buff.size(); }

    // 남은 미완성 데이터 강제 취소/초기화
    void clear() { m_buff.clear(); }

    char*   get_msg(){ return _msg;}

private:
    string          m_buff;
    char            _msg[1024]{};
    std::mutex      _mtxRecv;
    std::mutex      m_mtxRslt;
    deque<string>  m_rslt;
    std::thread     _thrd_parsing;
    bool            _is_continue{true};
};

}