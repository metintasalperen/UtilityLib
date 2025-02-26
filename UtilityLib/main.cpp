#include "ClientAsyncSelectCls.h"
#include "ClientEventSelectCls.h"
#include <iostream>

static int ClientBlockingLoop()
{
    addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string nodeName;
    std::string serviceName;

    std::cout << "Enter ip address: ";
    std::getline(std::cin, nodeName);

    std::cout << "Enter port: ";
    std::getline(std::cin, serviceName);

    UtilityLib::Network::SocketCls client;

    bool result = client.InitializeWinsock();
    if (!result)
    {
        std::cerr << "Cannot initalize winsock. Error code: " << client.GetLastSocketError() << std::endl;
        return -1;
    }

    result = client.GetAddressInfo(hints, nodeName, serviceName);
    if (!result)
    {
        std::cerr << "Cannot getaddrinfo. Error code: " << client.GetLastSocketError() << std::endl;
        return -1;
    }

    result = client.CreateSocket();
    if (!result)
    {
        std::cerr << "Cannot create socket. Error code: " << client.GetLastSocketError() << std::endl;
        return -1;
    }

    client.SetBlockingMode(UtilityLib::Network::BlockingModeEnum::NonBlocking);
    
    result = client.Connect();
    while (result == false)
    {
        if (client.GetLastSocketError() == WSAEWOULDBLOCK || client.GetLastSocketError() == WSAEALREADY)
        {
            std::cout << "Server is unreachable yet, will try again in 10 seconds..." << std::endl;
            Sleep(10000);
            result = client.Connect();
        }
        else
        {
            std::cerr << "Cannot connnect to server. Error code: " << 
                client.GetLastSocketError() << std::endl;
            return -1;
        }
    }

    client.SetBlockingMode(UtilityLib::Network::BlockingModeEnum::Blocking);
    std::cout << "Connected to server... " << nodeName << ":" << serviceName << std::endl;

    while (true)
    {
        std::string input;

        std::cout << "Enter message: ";
        std::getline(std::cin, input);

        int result = client.SendAll(input);
        if (result < 0)
        {
            std::cerr << "Error occurred while sending data. Error code: " << 
                WSAGetLastError() << std::endl;
            return -1;
        }

        std::string response;
        result = client.RecvAll(response);

        if (result < 0)
        {
            std::cerr << "Error occurred while receiving data. Error code: " << WSAGetLastError() << std::endl;
            return -1;
        }
        else if (result == 0)
        {
            std::cout << "Server closed connection... Closing app..." << std::endl;
            return 0;
        }

        std::cout << "Reply: " << response << std::endl;
    }
}

static int ClientAsyncSelectLoop()
{
    addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string nodeName;
    std::string serviceName;

    std::cout << "Enter ip address: ";
    std::getline(std::cin, nodeName);

    std::cout << "Enter port: ";
    std::getline(std::cin, serviceName);

    UtilityLib::Network::ClientAsyncSelectCls client;
    bool result = client.InitializeWinsock();
    if (!result)
    {
        std::cerr << "Cannot initalize winsock. Error code: " << client.GetLastSocketError() << std::endl;
        return -1;
    }

    result = client.GetAddressInfo(hints, nodeName, serviceName);
    if (!result)
    {
        std::cerr << "Cannot getaddrinfo. Error code: " << client.GetLastSocketError() << std::endl;
        return -1;
    }

    result = client.CreateSocket();
    if (!result)
    {
        std::cerr << "Cannot create socket. Error code: " << client.GetLastSocketError() << std::endl;
        return -1;
    }

    result = client.Connect();
    while (result == false)
    {
        if (client.GetLastSocketError() == WSAEWOULDBLOCK || client.GetLastSocketError() == WSAEALREADY)
        {
            std::cout << "Server is unreachable yet, will try again in 10 seconds..." << std::endl;
            Sleep(10000);
            result = client.Connect();
        }
        else
        {
            std::cerr << "Cannot connnect to server. Error code: " <<
                client.GetLastSocketError() << std::endl;
            return -1;
        }
    }

    std::thread clientThread(&UtilityLib::Network::ClientAsyncSelectCls::ThreadEntryPoint, &client);
    while (!client.GetHwnd())
    {
        Sleep(100);
    }

    std::string input;
    while (true)
    {
        std::cout << "Enter a string to send (or 'quit' to exit): ";
        std::getline(std::cin, input);
        if (input == "quit")
        {
            PostMessage(client.GetHwnd(), WM_QUIT_THREAD, NULL, reinterpret_cast<LPARAM>(input.c_str()));
            break;
        }

        if (!PostMessage(client.GetHwnd(), WM_SEND_STRING, NULL, reinterpret_cast<LPARAM>(input.c_str())))
        {
            std::cerr << "PostMessage Failed:" << client.GetLastWindowsError() << std::endl;
            break;
        }
        else
        {
            std::cout << "Received: " << client.GetBuffer() << std::endl;
        }
    }

    clientThread.join();
    return 0;
}

static int ClientEventSelectLoop()
{
    addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string nodeName;
    std::string serviceName;

    std::cout << "Enter ip address: ";
    std::getline(std::cin, nodeName);

    std::cout << "Enter port: ";
    std::getline(std::cin, serviceName);

    UtilityLib::Network::ClientEventSelectCls client;
    bool result = client.InitializeWinsock();
    if (!result)
    {
        std::cerr << "Cannot initalize winsock. Error code: " << client.GetLastSocketError() << std::endl;
        return -1;
    }

    result = client.GetAddressInfo(hints, nodeName, serviceName);
    if (!result)
    {
        std::cerr << "Cannot getaddrinfo. Error code: " << client.GetLastSocketError() << std::endl;
        return -1;
    }

    result = client.CreateSocket();
    if (!result)
    {
        std::cerr << "Cannot create socket. Error code: " << client.GetLastSocketError() << std::endl;
        return -1;
    }

    result = client.Connect();
    while (result == false)
    {
        if (client.GetLastSocketError() == WSAEWOULDBLOCK || client.GetLastSocketError() == WSAEALREADY)
        {
            std::cout << "Server is unreachable yet, will try again in 10 seconds..." << std::endl;
            Sleep(10000);
            result = client.Connect();
        }
        else
        {
            std::cerr << "Cannot connnect to server. Error code: " <<
                client.GetLastSocketError() << std::endl;
            return -1;
        }
    }

    std::thread clientThread(&UtilityLib::Network::ClientEventSelectCls::ThreadEntryPoint, &client);

    std::string input;
    while (true)
    {
        std::cout << "Enter a string to send to " << nodeName << " (or 'quit' to exit): ";
        std::getline(std::cin, input);
        if (input == "quit") break;

        client.PushMessage(input);
        std::cout << "Received: " << client.GetBuffer() << std::endl;
    }
}

int main()
{
    return ClientAsyncSelectLoop();
}