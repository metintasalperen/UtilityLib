#include "ClientAsyncSelectCls.h"
#include "ClientEventSelectCls.h"
#include <iostream>

static void SetupData(addrinfo& hints, std::string& nodeName, std::string& serviceName)
{
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::cout << "Enter ip address: ";
    std::getline(std::cin, nodeName);

    std::cout << "Enter port: ";
    std::getline(std::cin, serviceName);
}
static bool InitClient(UtilityLib::Network::SocketClientCls& client, const addrinfo& hints, const std::string& nodeName, const std::string& serviceName)
{
    ErrorEnum result = client.InitializeWinsock();
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Cannot initalize winsock. Error code: " << client.GetLastWsaError() << std::endl;
        return false;
    }

    result = client.GetAddressInfo(hints, nodeName, serviceName);
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Cannot getaddrinfo. Error code: " << client.GetLastWsaError() << std::endl;
        return false;
    }

    result = client.CreateSocket();
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Cannot create socket. Error code: " << client.GetLastWsaError() << std::endl;
        return false;
    }

    client.SetBlockingMode(UtilityLib::Network::BlockingMode::NonBlocking);

    result = client.Connect();
    int i = 0;
    while (result != ErrorEnum::Success && i < 15)
    {
        if (client.GetLastWsaError() == WSAEWOULDBLOCK ||
            client.GetLastWsaError() == WSAEALREADY)
        {
            std::cout << "Server is unreachable yet, will try again in 1 sec for " << 15 - i << " times..." << std::endl;
            i++;
            Sleep(1000);
            result = client.Connect();
        }
        else if (client.GetLastWsaError() == WSAEISCONN)
        {
            break;
        }
        else
        {
            break;
        }
    }

    if (result != ErrorEnum::Success)
    {
        std::cerr << "Could not connect to the server... Error code: " 
            << client.GetLastWsaError() << std::endl
            << "Shutting down..." << std::endl;

        return false;
    }
    else
    {
        client.SetBlockingMode(UtilityLib::Network::BlockingMode::Blocking);
        std::cout << "Connected to server... " << nodeName << ":" << serviceName << std::endl;

        return true;
    }
}

static int ClientBlockingLoop(UtilityLib::Network::SocketClientCls& client)
{
    while (true)
    {
        std::string input;
        std::cout << "Enter message to send or 'quit' to exit: ";
        std::getline(std::cin, input);

        if (input == "quit")
        {
            std::cout << "Goodbye..." << std::endl;
            return 0;
        }

        size_t len;
        auto result = client.Send(input, len);
        if (result == ErrorEnum::WinsockError)
        {
            std::cerr << "Error occurred while sending data. Error code: " <<
                client.GetLastWsaError() << std::endl;
            return -1;
        }
        else if (result == ErrorEnum::WinsockConnClosed)
        {
            std::cout << "Server disconnected. Shutting down..." << std::endl;
            return 0;
        }

        std::string response;
        result = client.Recv(response, response.size());

        if (result == ErrorEnum::WinsockError)
        {
            std::cerr << "Error occurred while receiving data. Error code: " <<
                client.GetLastWsaError() << std::endl;
            return -1;
        }
        else if (result == ErrorEnum::WinsockConnClosed)
        {
            std::cout << "Server disconnected. Shutting down..." << std::endl;
            return 0;
        }

        std::cout << "Reply: " << response << std::endl;
    }
}
static int ClientAsyncSelectLoop(UtilityLib::Network::ClientAsyncSelectCls& client)
{
    std::thread clientThread(&UtilityLib::Network::ClientAsyncSelectCls::ThreadEntryPoint, &client);
    while (!client.GetHwnd())
    {
        Sleep(100);
    }

    while (true)
    {
        std::string input;
        std::cout << "Enter message to send or 'quit' to exit: ";
        std::getline(std::cin, input);

        if (input == "quit")
        {
            PostMessage(client.GetHwnd(), WM_QUIT_THREAD, NULL, NULL);
            clientThread.join();
            return 0;
        }

        if (!PostMessage(client.GetHwnd(), WM_SEND_STRING, NULL, reinterpret_cast<LPARAM>(input.c_str())))
        {
            std::cerr << "Failed to send. Error code: " << client.GetLastWindowsError() << std::endl;
            return -1;
        }
        else
        {
            Sleep(10);
            std::cout << "Received: " << client.GetBuffer() << std::endl;
        }
    }

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
    auto result = client.InitializeWinsock();
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Cannot initalize winsock. Error code: " << client.GetLastWsaError() << std::endl;
        return -1;
    }

    result = client.GetAddressInfo(hints, nodeName, serviceName);
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Cannot getaddrinfo. Error code: " << client.GetLastWsaError() << std::endl;
        return -1;
    }

    result = client.CreateSocket();
    if (result != ErrorEnum::Success)
    {
        std::cerr << "Cannot create socket. Error code: " << client.GetLastWsaError() << std::endl;
        return -1;
    }

    result = client.Connect();
    while (result != ErrorEnum::Success)
    {
        if (client.GetLastWsaError() == WSAEWOULDBLOCK || client.GetLastWsaError() == WSAEALREADY)
        {
            std::cout << "Server is unreachable yet, will try again in 10 seconds..." << std::endl;
            Sleep(10000);
            result = client.Connect();
        }
        else
        {
            std::cerr << "Cannot connnect to server. Error code: " <<
                client.GetLastWsaError() << std::endl;
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
    addrinfo hints;
    std::string nodeName;
    std::string serviceName;
    SetupData(hints, nodeName, serviceName);

    UtilityLib::Network::ClientAsyncSelectCls client;
    bool result = InitClient(client, hints, nodeName, serviceName);
    if (!result)
    {
        return -1;
    }

    return ClientBlockingLoop(client);
}