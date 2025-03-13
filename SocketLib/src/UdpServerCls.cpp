#include "UdpServerCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        UdpServerCls::UdpServerCls() :
            UdpCommonCls()
        {
        }
        UdpServerCls::UdpServerCls(UdpCommonCls&& other) noexcept :
            UdpCommonCls(std::move(other))
        {
        }
        UdpServerCls& UdpServerCls::operator=(UdpCommonCls&& other) noexcept
        {
            UdpCommonCls::operator=(std::move(other));
            return *this;
        }
        UdpServerCls::UdpServerCls(UdpServerCls&& other) noexcept :
            UdpCommonCls(std::move(other))
        {
        }
        UdpServerCls& UdpServerCls::operator=(UdpServerCls&& other) noexcept
        {
            UdpCommonCls::operator=(std::move(other));
            return *this;
        }

        std::variant<WinsockError, UdpServerCls> UdpServerCls::Initialize(
            const addrinfo& hints,
            const std::string& ipAddress,
            const std::string& port,
            BlockingMode blockingMode)
        {
            auto initResult = UdpCommonCls::Initialize(hints, ipAddress, port, blockingMode);

            if (std::holds_alternative<WinsockError>(initResult))
            {
                return std::get<WinsockError>(initResult);
            }

            UdpServerCls udpServer(std::move(std::get<UdpCommonCls>(initResult)));

            WinsockError result = udpServer.Bind();

            if (result != WinsockError::Success)
            {
                return result;
            }

            return udpServer;
        }

        WinsockError UdpServerCls::Bind()
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

            LastWinsockError = WSAGetLastError();
            return WinsockError::CheckLastWinsockError;
        }
    }
}