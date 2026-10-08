#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iostream>
#include <cstdint>
#include <vector>
#include <atomic>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

constexpr const uint16_t PORT = 5555;

void handleClient(SOCKET clientSocket) {
	std::cout << "Client connected: ID=" << clientSocket << std::endl;

	char buffer[1024];

	int bytesReceived;

	while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
		std::cout << "Received: " << std::string(buffer, bytesReceived) << std::endl;
		send(clientSocket, "hello", 5, 0);
	}

	if (bytesReceived == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
	}

	closesocket(clientSocket);
}

int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
		return 1;
	}

	SOCKET server_socket { socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) };

	sockaddr_in server_addr{};
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
		std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
		closesocket(server_socket);
		WSACleanup();
		return 1;
	}

	while (true) {
		SOCKET client_socket{ accept(server_socket, nullptr, nullptr) };

		if (client_socket == INVALID_SOCKET) {
			std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
			closesocket(server_socket);
			WSACleanup();
			return 1;
		}

		std::thread clientThread(handleClient, client_socket);
		clientThread.detach();
	}

	return 0;
}
