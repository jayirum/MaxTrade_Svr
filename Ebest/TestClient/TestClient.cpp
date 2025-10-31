#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

void recv_loop(SOCKET s) {
    char buf[4096];
    while (true) {
        int rc = recv(s, buf, sizeof(buf) - 1, 0);
        if (rc <= 0) {
            std::cout << "Disconnected from server.\n";
            break;
        }
        buf[rc] = 0;
        std::cout << buf; // 서버가 tick이나 브로드캐스트 보냄
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

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(8010);
    inet_pton(AF_INET, "127.0.0.1", &srv.sin_addr); // 로컬 테스트용

    if (connect(s, (sockaddr*)&srv, sizeof(srv)) == SOCKET_ERROR) {
        std::cerr << "connect failed, err=" << WSAGetLastError() << "\n";
        closesocket(s);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server. Type messages and press Enter.\n";
    std::thread r(recv_loop, s);

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
