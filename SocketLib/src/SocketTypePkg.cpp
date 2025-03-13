#include "SocketTypePkg.h"

namespace UtilityLib
{
    namespace Socket
    {
        consteval addrinfo GetDefaultUdpHints()
        {
            addrinfo hints{};
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_DGRAM;
            hints.ai_protocol = IPPROTO_UDP;
            return hints;
        }
        consteval addrinfo GetDefaultTcpHints()
        {
            addrinfo hints{};
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;
            return hints;
        }
    }
}