#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

constexpr const uint16_t PORT = 5555;
constexpr const char* SERVER_IP = "84.171.147.101";

int main() {
    WSADATA wsaData{};
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << '\n';
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << '\n';
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
    
    result = connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

    if (result == SOCKET_ERROR) {
        std::cerr << "connect failed: " << WSAGetLastError() << '\n';

        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected!\n";

    int sent = send(sock, "htrhjrhrth", static_cast<const int>(strlen("htrhjrhrth")), 0);

    if (sent == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << '\n';
    }

    char buffer[1024];

    int received = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (received > 0) {
        buffer[received] = '\0';
        std::cout << "Server: " << buffer << '\n';
    }
    else if (received == 0) {
        std::cout << "Server closed connection\n";
    }
    else {
        std::cerr << "recv failed: " << WSAGetLastError() << '\n';
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}
