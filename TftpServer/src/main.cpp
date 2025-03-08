#include <iostream>
#include "SocketPkg.h"

#define ALPEREN

int main()
{
    std::cout << "Hello from Server" << std::endl;
    
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    std::string ipAddr;
    std::cout << "Enter IP Address: ";
    std::getline(std::cin, ipAddr);

    std::string port;
    std::cout << "Enter Port: ";
    std::getline(std::cin, port);

    UtilityLib::Network::SocketServerCls server;

    ErrorEnum result = server.InitializeWinsock();
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Cannot Initialize Winsock. Error code: " << server.GetLastWsaError() << std::endl;
        return -1;
    }

    result = server.GetAddressInfo(hints, ipAddr, port);
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Error during getaddrinfo. Error code: " << server.GetLastWsaError() << std::endl;
        return -1;
    }

    result = server.CreateSocket();
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Cannot create socket. Error code: " << server.GetLastWsaError() << std::endl;
        return -1;
    }

    result = server.Bind();
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Cannot bind. Error code: " << server.GetLastWsaError() << std::endl;
        return -1;
    }

    server.SetBlockingMode(UtilityLib::Network::BlockingMode::Blocking);

    std::cout << "UDP Echo server running on " << ipAddr << ":" << port << "..." << std::endl;

    while (true)
    {
        std::string receivedMsg;
        result = server.RecvFrom(receivedMsg, 4096);

        if (result != ErrorEnum::Success)
        {
            std::cerr << "recvfrom failed. Error code: " << server.GetLastWsaError() << std::endl;
            continue;
        }

        size_t sentBytes = 0;
        result = server.SendTo(receivedMsg, sentBytes);

        if (result != ErrorEnum::Success)
        {
            std::cerr << "sendto failed. Error code: " << server.GetLastWsaError() << std::endl;
            continue;
        }

        std::cout << receivedMsg << " echoed back (" << sentBytes << " bytes)" << std::endl;
    }

    return 0;
}