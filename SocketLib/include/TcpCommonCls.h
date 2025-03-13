#ifndef TCPCOMMONCLS_H
#define TCPCOMMONCLS_H

#include "SocketCommonCls.h"
#include <variant>

namespace UtilityLib
{
    namespace Socket
    {
        class TcpCommonCls : public SocketCommonCls
        {
        protected:
            TcpCommonCls();

        public:
            static std::variant<WinsockError, TcpCommonCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);

            TcpCommonCls(TcpCommonCls&& other) noexcept;
            TcpCommonCls& operator=(TcpCommonCls&& other) noexcept;

            TcpCommonCls(const TcpCommonCls&) = delete;
            TcpCommonCls& operator=(const TcpCommonCls&) = delete;

            WinsockError Send(const std::string& buffer, size_t bufferLen, size_t& sentByteCount);
            WinsockError Recv(std::string& buffer, size_t bufferLen, size_t& recvByteCount);
        };
    }
}

#endif