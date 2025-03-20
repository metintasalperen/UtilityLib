#ifndef SOCKETTYPEPKG_H
#define SOCKETTYPEPKG_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <string>

#include "StringPkg.h"

namespace UtilityLib
{
    namespace Socket
    {
        enum class BlockingMode
        {
            Blocking = 0,
            NonBlocking = 1
        };
        enum class WinsockError
        {
            Success = 0,
            InvalidIpAddress,      // Provided IP address is invalid
            InvalidPort,           // Provided port is invalid
            BufferTooLong,         // Buffer length is too long, try using smaller length than INT_MAX
            BufferLengthIsZero,    // Buffer length is 0, what are you trying to do?
            NotInitialized,        // Winsock2 socket is not initialized
            OutOfMemory,           // Cannot allocate memory, go buy some more RAM man it is 2025
            CheckLastWinsockError, // Internal Winsock2 error. Call GetLastWinsockError() to receive specific error code, then check Microsoft's documentation
        };

        // This can be passed to Initialize() methods of TcpClientCls and TcpServerCls as hints parameter
        // 
        // Sets ai_family to AF_INET, 
        //      ai_socktype to SOCK_STREAM, 
        //      ai_protocol to IPPROTO_TCP
        // 
        // Use other hints if you need anything else...
        consteval addrinfo GetDefaultTcpHints()
        {
            addrinfo hints{ 0 };
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            return hints;
        }

        // Internal function, do not use this directly unless you really want to
        // Inputs are assumed as valid port and ip address
        // Do not pass invalid port or ip address if you really really want to use this
        inline sockaddr_in CreateSockaddrIn(const std::string& port, const std::string& ipAddress = "")
        {
            sockaddr_in addr{ 0 };

            addr.sin_family = AF_INET;
            
            UtilityLib::String::StringToIntegral<USHORT>(port, addr.sin_port);
            addr.sin_port = htons(addr.sin_port);

            if (ipAddress == "")
            {
                addr.sin_addr.S_un.S_addr = INADDR_ANY;
            }
            else
            {
                auto ipBlocks = UtilityLib::String::Divide(ipAddress, '.');
                UCHAR ipBlock[4]{};
                UtilityLib::String::StringToIntegral<UCHAR>(ipBlocks.at(0), ipBlock[0]);
                UtilityLib::String::StringToIntegral<UCHAR>(ipBlocks.at(1), ipBlock[1]);
                UtilityLib::String::StringToIntegral<UCHAR>(ipBlocks.at(2), ipBlock[2]);
                UtilityLib::String::StringToIntegral<UCHAR>(ipBlocks.at(3), ipBlock[3]);
                addr.sin_addr.S_un.S_un_b = { ipBlock[0], ipBlock[1], ipBlock[2], ipBlock[3] };
            }
            return addr;
        }

        // Internal function, do not use this directly unless you really want to
        inline std::string SockaddrInToIpAddress(sockaddr_in* ptr)
        {
            std::vector<std::string> ipBlocks;
            ipBlocks.push_back(UtilityLib::String::IntegralToString<UCHAR>(ptr->sin_addr.S_un.S_un_b.s_b1));
            ipBlocks.push_back(UtilityLib::String::IntegralToString<UCHAR>(ptr->sin_addr.S_un.S_un_b.s_b2));
            ipBlocks.push_back(UtilityLib::String::IntegralToString<UCHAR>(ptr->sin_addr.S_un.S_un_b.s_b3));
            ipBlocks.push_back(UtilityLib::String::IntegralToString<UCHAR>(ptr->sin_addr.S_un.S_un_b.s_b4));

            return UtilityLib::String::Join(ipBlocks, '.');
        }
    };
}

#endif