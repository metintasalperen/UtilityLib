#ifndef SOCKETCLS_H
#define SOCKETCLS_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <string>
#include <vector>
#include <mutex>

#include "StringPkg.h"
#include "ErrorPkg.h"

using namespace UtilityLib::Error;

#pragma comment(lib, "Ws2_32.lib")

namespace UtilityLib
{
    namespace Network
    {
        enum class BlockingMode
        {
            Blocking = 0,
            NonBlocking = 1
        };
        class SocketCommonCls : public WSAErrorCls
        {
        protected:
            SOCKET Sock;
            addrinfo Hints;
            addrinfo* AddressInfoResults;
            std::string IpAddress;
            std::string Port;

        public:
            SocketCommonCls();
            SocketCommonCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port);
            ~SocketCommonCls();
            ErrorEnum InitializeWinsock();
            ErrorEnum CleanupWinsock();
            void SetHints(const addrinfo& hints);
            void SetIpAddress(const std::string& ipAddress);
            void SetPort(const std::string& port);
            ErrorEnum CreateSocket();
            ErrorEnum CloseSocket();
            ErrorEnum GetAddressInfo();
            ErrorEnum GetAddressInfo(const addrinfo& hints, const std::string& ipAddress, const std::string& port);
            ErrorEnum Recv(std::string& buffer, size_t bufferLength);
            ErrorEnum Send(const std::string& buffer, size_t& bytesSent);
            ErrorEnum SetBlockingMode(BlockingMode mode);
            virtual ErrorEnum SendTo(const std::string& buffer, size_t& bytesSent) = 0;
            virtual ErrorEnum RecvFrom(std::string& buffer, size_t bufferLength) = 0;
            //virtual ErrorEnum SendTo(const char* buffer, size_t bufferLength, size_t& bytesSent) = 0;
        };
        class SocketClientCls : public SocketCommonCls
        {
        public:
            SocketClientCls();
            SocketClientCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port);

            ErrorEnum Connect(); 
            ErrorEnum SendTo(const std::string& buffer, size_t& bytesSent);
            ErrorEnum RecvFrom(std::string& buffer, size_t bufferLength);
            ErrorEnum SendTo(const char* buffer, size_t bufferLength, size_t& bytesSent);
        };
        class SocketServerCls : public SocketCommonCls
        {
        public:
            sockaddr_in RemoteAddress;

            SocketServerCls();
            SocketServerCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port);

            ErrorEnum Bind();
            ErrorEnum Listen();
            ErrorEnum Accept();
            ErrorEnum SendTo(const std::string& buffer, size_t& bytesSent);
            ErrorEnum RecvFrom(std::string& buffer, size_t bufferLength);
        };
    };
};

#endif