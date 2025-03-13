#include "TcpServerCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        TcpServerCls::TcpServerCls() :
            TcpCommonCls()
        {
        }
        TcpServerCls::TcpServerCls(TcpCommonCls&& other) noexcept :
            TcpCommonCls(std::move(other))
        {
        }
        TcpServerCls& TcpServerCls::operator=(TcpCommonCls&& other) noexcept
        {
            TcpCommonCls::operator=(std::move(other));
            return *this;
        }
        TcpServerCls::TcpServerCls(TcpServerCls&& other) noexcept :
            TcpCommonCls(std::move(other))
        {
        }
        TcpServerCls& TcpServerCls::operator=(TcpServerCls&& other) noexcept
        {
            TcpCommonCls::operator=(std::move(other));
            return *this;
        }

        std::variant<WinsockError, TcpServerCls> TcpServerCls::Initialize(
            const addrinfo& hints,
            const std::string& ipAddress,
            const std::string& port,
            BlockingMode blockingMode)
        {
            auto initResult = TcpCommonCls::Initialize(hints, ipAddress, port, blockingMode);
            if (std::holds_alternative<WinsockError>(initResult))
            {
                return std::get<WinsockError>(initResult);
            }
            TcpServerCls tcpServer = std::move(std::get<TcpCommonCls>(initResult));
            WinsockError result = tcpServer.Bind();
            if (result != WinsockError::Success)
            {
                return result;
            }
            return tcpServer;
        }

        WinsockError TcpServerCls::Bind()
        {
            if (AddressInfoResults == nullptr ||
                Sock == INVALID_SOCKET)
            {
                return WinsockError::NotInitialized;
            }
            for (const addrinfo* ptr = AddressInfoResults; ptr != nullptr; ptr = ptr->ai_next)
            {
                int iResult = bind(Sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
                if (iResult != SOCKET_ERROR)
                {
                    return WinsockError::Success;
                }
            }
            return WinsockError::CheckLastWinsockError;
        }
        WinsockError TcpServerCls::Listen(int backlog)
        {
            if (Sock == INVALID_SOCKET)
            {
                return WinsockError::NotInitialized;
            }
            int iResult = listen(Sock, backlog);
            if (iResult == SOCKET_ERROR)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }
            return WinsockError::Success;
        }
        std::variant<WinsockError, TcpCommonCls> TcpServerCls::Accept()
        {
            if (Sock == INVALID_SOCKET)
            {
                return WinsockError::NotInitialized;
            }
            SOCKET clientSock = accept(Sock, nullptr, nullptr);
            if (clientSock == INVALID_SOCKET)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }

            auto client = TcpCommonCls::Initialize(Hints, IpAddress, Port);
            return WinsockError::Success;
        }

    }
}