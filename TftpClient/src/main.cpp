#include <iostream>
#include "SocketPkg.h"
#include "TftpClientCls.h"

int main()
{
    std::string ipAddr;
    std::cout << "Ip Address of TFTP Server: ";
    std::getline(std::cin, ipAddr);
    
    auto tftpClient = Tftp::TftpClientCls::Initialize(ipAddr);
    if (tftpClient)
    {
        std::string readOrWrite;
        std::cout << "Read or Write (r/w): ";
        std::getline(std::cin, readOrWrite);

        if (readOrWrite == "r")
        {
            std::string filename;
            std::cout << "File name to read: ";
            std::getline(std::cin, filename);

            std::string path;
            std::cout << "Where to save: ";
            std::getline(std::cin, path);

            auto result = tftpClient->ReadFile(filename, path, Tftp::Mode::NetAscii);

            if (result == Tftp::TftpErrorCodes::Success)
            {
                std::cout << "File received and successfully saved to " << path << "\n";
                return 0;
            }
            else
            {
                std::cerr << "Some error occurred while reading... Error code: " << Tftp::ERROR_MESSAGES.at(result) << "\n";
                return -1;
            }
        }
        else if (readOrWrite == "w")
        {
            std::string filename;
            std::cout << "File name to write: ";
            std::getline(std::cin, filename);

            std::string path;
            std::cout << "Path to file: ";
            std::getline(std::cin, path);

            auto result = tftpClient->WriteFile(filename, path, Tftp::Mode::NetAscii);
            if (result == Tftp::TftpErrorCodes::Success)
            {
                std::cout << "File sent successfully" << std::endl;
                return 0;
            }
            else
            {
                std::cerr << "Some error occurred while writing... Error code: " << Tftp::ERROR_MESSAGES.at(result) << "\n";
                return -1;
            }
        }
        else
        {
            std::cout << "Try again" << std::endl;
        }
    }
    else
    {
        std::cerr << "Cannot initialize TFTP client" << std::endl;
        return -1;
    }
}