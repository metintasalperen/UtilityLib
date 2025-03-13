#ifndef UDPCOMMONCLS_H
#define UDPCOMMONCLS_H

#include "SocketCommonCls.h"
#include <variant>

namespace UtilityLib
{
    namespace Socket
    {
        class UdpCommonCls : public SocketCommonCls
        {
        protected:
            UdpCommonCls();
        public:
            UdpCommonCls(UdpCommonCls&& other) noexcept;
            UdpCommonCls& operator=(UdpCommonCls&& other) noexcept;

            UdpCommonCls(const UdpCommonCls&) = delete;
            UdpCommonCls& operator=(const UdpCommonCls&) = delete;

            static std::variant<WinsockError, UdpCommonCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);

            WinsockError RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount);
            WinsockError SendTo(const std::string& buffer, size_t bufferLen, size_t& sentByteCount);
        };
    }
}

#endif