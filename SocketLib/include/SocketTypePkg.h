#ifndef SOCKETTYPEPKG_H
#define SOCKETTYPEPKG_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>

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
            InvalidIpAddress,
            InvalidPort,
            BufferTooLong,
            NotInitialized,
            OutOfMemory,
            CheckLastWinsockError,
        };

        consteval addrinfo GetDefaultUdpHints();
        consteval addrinfo GetDefaultTcpHints();
    };
}

#endif