#ifndef TCPSERVERCLS_H
#define TCPSERVERCLS_H

#include "TcpCommonCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        class TcpServerCls : public TcpCommonCls
        {
        private:
            TcpServerCls();
            TcpServerCls(TcpCommonCls&& other) noexcept;
            TcpServerCls& operator=(TcpCommonCls&& other) noexcept;

            WinsockError Bind();

        public:
            static std::variant<WinsockError, TcpServerCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);

            TcpServerCls(TcpServerCls&& other) noexcept;
            TcpServerCls& operator=(TcpServerCls&& other) noexcept;

            TcpServerCls(const TcpServerCls&) = delete;
            TcpServerCls& operator=(const TcpServerCls&) = delete;

            WinsockError Listen(int backlog);
            std::variant<WinsockError, TcpCommonCls> Accept();
        };
    }
}

#endif