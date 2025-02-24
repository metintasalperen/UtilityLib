#include "UtilityLib.h"

int main()
{
    addrinfo hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    std::string nodeName = "127.0.0.1";
    std::string serviceName = "12345";

    UtilityLib::NetworkIO::ClientEventSelectCls client(hints, nodeName, serviceName);
    if (!client.IsConnected)
    {
        std::cerr << "Some error occurred" << WSAGetLastError() << std::endl;
        return -1;
    };

    std::thread clientThread(&UtilityLib::NetworkIO::ClientEventSelectCls::ThreadEntryPoint, &client);

    std::string input;
    std::cout << "Enter a string to send (or 'quit' to exit): ";
    while (true)
    {
        std::getline(std::cin, input);
        if (input == "quit") break;

        client.PushMessage(input);
    }

    client.Stop();
    clientThread.join();

    return 0;
}