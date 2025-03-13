#include "TcpClientCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        TcpClientCls::TcpClientCls() :
            TcpCommonCls()
        {
        }
        TcpClientCls::TcpClientCls(TcpCommonCls&& other) noexcept :
            TcpCommonCls(std::move(other))
        {
        }
        TcpClientCls& TcpClientCls::operator=(TcpCommonCls&& other) noexcept
        {
            TcpCommonCls::operator=(std::move(other));
            return *this;
        }
        TcpClientCls::TcpClientCls(TcpClientCls&& other) noexcept :
            TcpCommonCls(std::move(other))
        {
        }
        TcpClientCls& TcpClientCls::operator=(TcpClientCls&& other) noexcept
        {
            TcpCommonCls::operator=(std::move(other));
            return *this;
        }

        std::variant<WinsockError, TcpClientCls> TcpClientCls::Initialize(
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
            TcpClientCls tcpClient = std::move(std::get<TcpCommonCls>(initResult));
            WinsockError result = tcpClient.Connect();
            if (result != WinsockError::Success)
            {
                return result;
            }
            return tcpClient;
        }
        
        WinsockError TcpClientCls::Connect()
        {
            if (AddressInfoResults == nullptr ||
                Sock == INVALID_SOCKET)
            {
                return WinsockError::NotInitialized;
            }
            for (const addrinfo* ptr = AddressInfoResults; ptr != nullptr; ptr = ptr->ai_next)
            {
                int iResult = connect(Sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
                if (iResult != SOCKET_ERROR)
                {
                    return WinsockError::Success;
                }
            }
            return WinsockError::Success;
        }
    }
}