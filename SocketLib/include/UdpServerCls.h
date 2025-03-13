#ifndef UDPSERVERCLS_H
#define UDPSERVERCLS_H

#include "UdpCommonCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        class UdpServerCls : public UdpCommonCls
        {
        private:
            UdpServerCls();
            UdpServerCls(UdpCommonCls&& other) noexcept;
            UdpServerCls& operator=(UdpCommonCls&& other) noexcept;

            WinsockError Bind();
        public:
            static std::variant<WinsockError, UdpServerCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);

            UdpServerCls(UdpServerCls&& other) noexcept;
            UdpServerCls& operator=(UdpServerCls&& other) noexcept;

            UdpServerCls(const UdpServerCls&) = delete;
            UdpServerCls& operator=(const UdpServerCls&) = delete;
        };
    }
}

#endif