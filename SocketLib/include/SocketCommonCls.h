#ifndef SOCKETCOMMONCLS_H
#define SOCKETCOMMONCLS_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <string>

#include "SocketTypePkg.h"
#include "WinsockInitializerCls.h"
#include "StringPkg.h"

namespace UtilityLib
{
    namespace Socket
    {
        // Common attributes and methods for UDP and TCP
        class SocketCommonCls
        {
        protected:
            SOCKET Sock;
            addrinfo Hints;
            addrinfo* AddressInfoResults;
            std::string IpAddress;
            std::string Port;
            int LastWinsockError;

            SocketCommonCls();
            SocketCommonCls(SocketCommonCls&& other) noexcept;
            SocketCommonCls& operator=(SocketCommonCls&& other) noexcept;

            SOCKET GetSocket() const;
            addrinfo GetHints() const;
            addrinfo* GetAddressInfoResults() const;
            std::string GetIpAddress() const;
            std::string GetPort() const;

            void SetHints(const addrinfo& hints);
            bool SetIpAddress(const std::string& ipAddress);
            bool SetPort(const std::string& port);

            WinsockError InitializeWinsock();
            WinsockError CleanupWinsock();
            WinsockError GetAddressInfo();
            WinsockError CreateSocket();
            WinsockError CloseSocket();
            WinsockError SetBlockingMode(BlockingMode mode);

        public:
            ~SocketCommonCls();
            int GetLastWinsockError() const;

            SocketCommonCls(const SocketCommonCls&) = delete;
            SocketCommonCls& operator=(const SocketCommonCls&) = delete;
        };
    }
}

#endif