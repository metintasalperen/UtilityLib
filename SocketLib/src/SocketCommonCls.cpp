#include "SocketCommonCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        SocketCommonCls::SocketCommonCls() :
            Sock(INVALID_SOCKET),
            AddressInfoResults(nullptr),
            LastWinsockError(0),
            Hints{ 0 }
        {
        }
        SocketCommonCls::SocketCommonCls(SocketCommonCls&& other) noexcept :
            Sock(other.Sock),
            Hints(other.Hints),
            AddressInfoResults(other.AddressInfoResults),
            LastWinsockError(other.LastWinsockError),
            IpAddress(std::move(other.IpAddress)),
            Port(std::move(other.Port))
        {
            other.Sock = INVALID_SOCKET;
            other.AddressInfoResults = nullptr;
        }
        SocketCommonCls& SocketCommonCls::operator=(SocketCommonCls&& other) noexcept
        {
            if (this != &other)
            {
                if (Sock != INVALID_SOCKET)
                {
                    closesocket(Sock);
                }
                if (AddressInfoResults != nullptr)
                {
                    freeaddrinfo(AddressInfoResults);
                }

                Sock = other.Sock;
                Hints = other.Hints;
                AddressInfoResults = other.AddressInfoResults;
                LastWinsockError = other.LastWinsockError;
                IpAddress = std::move(other.IpAddress);
                Port = std::move(other.Port);

                other.Sock = INVALID_SOCKET;
                other.AddressInfoResults = nullptr;
            }

            return *this;
        }
        SocketCommonCls::~SocketCommonCls()
        {
            if (AddressInfoResults != nullptr)
            {
                freeaddrinfo(AddressInfoResults);
            }

            if (Sock != INVALID_SOCKET)
            {
                closesocket(Sock);
            }

            CleanupWinsock();
        }

        SOCKET SocketCommonCls::GetSocket() const
        {
            return Sock;
        }
        addrinfo SocketCommonCls::GetHints() const
        {
            return Hints;
        }
        addrinfo* SocketCommonCls::GetAddressInfoResults() const
        {
            return AddressInfoResults;
        }
        std::string SocketCommonCls::GetIpAddress() const
        {
            return IpAddress;
        }
        std::string SocketCommonCls::GetPort() const
        {
            return Port;
        }
        int SocketCommonCls::GetLastWinsockError() const
        {
            return LastWinsockError;
        }

        void SocketCommonCls::SetHints(const addrinfo& hints)
        {
            Hints = hints;
        }
        bool SocketCommonCls::SetIpAddress(const std::string& ipAddress)
        {
            bool result = UtilityLib::String::ValidateIpAddress(ipAddress);
            if (result)
            {
                IpAddress = ipAddress;
            }
            return result;
        }
        bool SocketCommonCls::SetPort(const std::string& port)
        {
            bool result = UtilityLib::String::ValidatePort(port);
            if (result)
            {
                Port = port;
            }
            return result;
        }

        WinsockError SocketCommonCls::InitializeWinsock()
        {
            WinsockError result = WinsockError::Success;

            if (WinsockInitializerCls::GetInstanceCount() == 0)
            {
                WSADATA wsaData;
                int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
                if (iResult == SOCKET_ERROR)
                {
                    LastWinsockError = WSAGetLastError();
                    result = WinsockError::CheckLastWinsockError;
                }
            }

            if (result == WinsockError::Success)
            {
                WinsockInitializerCls::IncrementInstanceCount();
            }

            return result;
        }
        WinsockError SocketCommonCls::CleanupWinsock()
        {
            WinsockError result = WinsockError::Success;

            if (WinsockInitializerCls::GetInstanceCount() == 1)
            {
                int iResult = WSACleanup();
                if (iResult == SOCKET_ERROR)
                {
                    LastWinsockError = WSAGetLastError();
                    result = WinsockError::CheckLastWinsockError;
                }
            }

            if (result == WinsockError::Success)
            {
                WinsockInitializerCls::DecrementInstanceCount();
            }

            return result;
        }
        WinsockError SocketCommonCls::GetAddressInfo()
        {
            WinsockError result = WinsockError::Success;

            if (AddressInfoResults != nullptr)
            {
                freeaddrinfo(AddressInfoResults);
                AddressInfoResults = nullptr;
            }

            int iResult = getaddrinfo(IpAddress.c_str(), Port.c_str(), &Hints, &AddressInfoResults);

            if (iResult != 0)
            {
                LastWinsockError = WSAGetLastError();
                result = WinsockError::CheckLastWinsockError;
            }

            return result;
        }
        WinsockError SocketCommonCls::CreateSocket()
        {
            for (const addrinfo* ptr = AddressInfoResults; ptr != nullptr; ptr = ptr->ai_next)
            {
                Sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
                if (Sock != INVALID_SOCKET)
                {
                    return WinsockError::Success;
                }
            }

            LastWinsockError = WSAGetLastError();
            return WinsockError::CheckLastWinsockError;
        }
        WinsockError SocketCommonCls::CloseSocket()
        {
            WinsockError result = WinsockError::Success;
            if (Sock != INVALID_SOCKET)
            {
                int iResult = closesocket(Sock);
                if (iResult == SOCKET_ERROR)
                {
                    LastWinsockError = WSAGetLastError();
                    result = WinsockError::CheckLastWinsockError;
                }
                else
                {
                    Sock = INVALID_SOCKET;
                }
            }
            return result;
        }
        WinsockError SocketCommonCls::SetBlockingMode(BlockingMode mode)
        {
            u_long blockingMode = static_cast<u_long>(mode);

            int iResult = ioctlsocket(Sock, FIONBIO, &blockingMode);
            if (iResult == SOCKET_ERROR)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }

            return WinsockError::Success;
        }

    }
}