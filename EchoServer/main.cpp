#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib") // Link Ws2_32.lib automatically in MSVC

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return 1;
    }

    // Create a listening socket
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Bind the socket to IP and port
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345); // Port 12345
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    std::cout << "Echo server listening on 127.0.0.1:12345..." << std::endl;

    // Accept a client connection
    SOCKET clientSock = accept(listenSock, NULL, NULL);
    if (clientSock == INVALID_SOCKET) {
        std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }
    std::cout << "Client connected" << std::endl;

    // Echo loop
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
            break;
        }
        if (bytesReceived == 0) {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

        buffer[bytesReceived] = '\0'; // Null-terminate for printing
        std::cout << "Received: " << buffer << std::endl;

        int bytesSent = send(clientSock, buffer, bytesReceived, 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "send failed: " << WSAGetLastError() << std::endl;
            break;
        }
        std::cout << "Echoed back: " << buffer << " (" << bytesSent << " bytes)" << std::endl;
    }

    // Cleanup
    closesocket(clientSock);
    closesocket(listenSock);
    WSACleanup();
    std::cout << "Server shutting down" << std::endl;
    return 0;
}