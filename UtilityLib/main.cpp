#include "UtilityLib.h"

int main()
{
    addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    std::string nodeName = "127.0.0.1";
    std::string serviceName = "12345";

    UtilityLib::NetworkIO::Client client(hints, nodeName, serviceName);
    if (!client.IsConnected)
    {
        std::cerr << "Some error occurred" << WSAGetLastError() << std::endl;
        return -1;
    };

    std::thread clientThread(&UtilityLib::NetworkIO::Client::ThreadEntryPoint, &client);
    while (!client.Hwnd)
    {
        Sleep(100);
    }

    std::string input;
    while (true)
    {
        std::cout << "Enter a string to send (or 'quit' to exit): ";
        std::getline(std::cin, input);
        if (input == "quit") break;

        if (!PostMessage(client.Hwnd, WM_SEND_STRING, NULL, reinterpret_cast<LPARAM>(input.c_str())))
        {
            std::cerr << "PostMessage Failed:" << GetLastError() << std::endl;
            break;
        }
    }

    clientThread.join();

    return 0;
}