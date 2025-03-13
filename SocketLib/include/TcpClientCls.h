#ifndef TCPCLIENTCLS_H
#define TCPCLIENTCLS_H

#include "TcpCommonCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        class TcpClientCls : public TcpCommonCls
        {
        private:
            TcpClientCls();
            TcpClientCls(TcpCommonCls&& other) noexcept;
            TcpClientCls& operator=(TcpCommonCls&& other) noexcept;

            WinsockError Connect();
        public:
            static std::variant<WinsockError, TcpClientCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);

            TcpClientCls(TcpClientCls&& other) noexcept;
            TcpClientCls& operator=(TcpClientCls&& other) noexcept;

            TcpClientCls(const TcpClientCls&) = delete;
            TcpClientCls& operator=(const TcpClientCls&) = delete;
        };
    }
}

#endif