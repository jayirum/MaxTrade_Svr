#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <json.hpp>
#include <random>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

string      g_ip;
int         g_port;
sockaddr_in g_sockAddr{};
int         g_client_cnt;

int getRandomInt(int min , int max )
{
    static std::random_device rd;          // 하드웨어 시드
    static std::mt19937 gen(rd());         // Mersenne Twister 엔진
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

void disconnect(SOCKET s)
{
    linger lg{ TRUE, 0 }; setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&lg, sizeof(lg));
    closesocket(s); s = INVALID_SOCKET;
}

void sock_action(int idx, string& json_str, int max_cnt)
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        printf("[%d]socket failed\n", idx);
        return ;
    }

    if (connect(s, (sockaddr*)&g_sockAddr, sizeof(g_sockAddr)) == SOCKET_ERROR) {
        printf("[%d]connect failed(%d)\n", idx, WSAGetLastError());
        closesocket(s);
        return ;
    }

    int len = send(s, json_str.c_str(), (int)json_str.size(), 0);

    char buf[4096];
    int loop = 0;
    for(loop=0; loop< max_cnt; loop++)
    {
        int rc = recv(s, buf, sizeof(buf) - 1, 0);
        if (rc <= 0) {
            printf("[%d]Disconnected from server.\n", idx);
            break;
        }
        buf[rc] = 0;

        if (loop % g_client_cnt  == 0) {
            printf("[%d]%.150s\n", idx, buf);
        }
    }
    printf("[%d]Disconnect\n", idx);
    disconnect(s);
}

void thrd_recv(int idx)
{

    nlohmann::json j;
    j["symbol"] = "all";
    j["timeframe"] = 0;
    j["client_type"] = "server";

    string json_str = std::move(j.dump());
    json_str += "\n";


    while(true){
        int max_cnt = getRandomInt(10, 100);
        sock_action(idx, json_str, max_cnt);
        Sleep(5000);
    }

    
}


int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    printf("input server ip. local ip is 127.0.0.1\n");
    std::cin >> g_ip;

    printf("input server port.\n");
    std::cin >> g_port;

    g_sockAddr.sin_family = AF_INET;
    g_sockAddr.sin_port = htons(g_port);
    inet_pton(AF_INET, g_ip.c_str(), &g_sockAddr.sin_addr); // 로컬 테스트용

    printf("input client count\n");
    std::cin >> g_client_cnt;

    vector<std::thread> thrd_list;
    for (int i = 0; i < g_client_cnt; i++) {
        thrd_list.push_back(std::thread(thrd_recv, i));
    }

    for (auto& it : thrd_list) {
        if(it.joinable())   it.join();
    }

    return 0;
}

/*
void recv_loop(SOCKET s) {
    char buf[4096];
    while (true) {
        int rc = recv(s, buf, sizeof(buf) - 1, 0);
        if (rc <= 0) {
            std::cout << "Disconnected from server.\n";
            break;
        }
        buf[rc] = 0;
        //if( strncmp(buf, "RA001", 5)==0)
        //    printf("[시세]%.50s\n", buf);
        //else
        //    printf("[호가]%.50s\n", buf);

        printf("%.150s\n", buf);
    }
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        std::cerr << "socket failed\n";
        return 1;
    }

    INT port=0;
    std::string ip;
    printf("input server ip. local ip is 127.0.0.1\n");
    std::cin >> ip;
    
    printf("input server port. kf:9010, of:9020\n");
    std::cin >> port;

    printf("input client type. server or cient\n");
    std::string json_str;
    std::cin >> json_str;
    if(json_str.compare("server")==0){
        json_str = R"({"symbol": "all","timeframe": 0,"client_type": "server"})";
    }
    else {
        json_str = R"({"symbol": "NQZ25","timeframe": 3,"client_type": "client"})";
    }

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &srv.sin_addr); // 로컬 테스트용

    if (connect(s, (sockaddr*)&srv, sizeof(srv)) == SOCKET_ERROR) {
        std::cerr << "connect failed, err=" << WSAGetLastError() << "\n";
        closesocket(s);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Connected to server. Type messages and press Enter.\n";
    std::thread r(recv_loop, s);


    send(s, json_str.c_str(), (int)json_str.size(), 0);


    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "/quit" || line == "/exit") break;
        line.append("\r\n");  // 서버가 CRLF 기준으로 파싱하므로
        send(s, line.c_str(), (int)line.size(), 0);
    }

    shutdown(s, SD_BOTH);
    closesocket(s);
    if (r.joinable()) r.join();
    WSACleanup();
    return 0;
}
*/