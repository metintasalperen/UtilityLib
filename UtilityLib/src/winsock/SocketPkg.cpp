#include "SocketPkg.h"

namespace UtilityLib
{
    namespace Network
    {
        uint32_t WinsockInitializerCls::InstanceCount = 0;

        
        SocketCommonCls::SocketCommonCls() :
            Sock(INVALID_SOCKET),
            AddressInfoResults(nullptr),
            LastWinsockError(0),
            Hints{ 0 }
        { }
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
            bool result = UtilityLib::String::IsIntegral(port);
            if (result)
            {
                Port = port;
            }
            return result;
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
                // Free current resources
                if (Sock != INVALID_SOCKET)
                {
                    closesocket(Sock);
                }
                if (AddressInfoResults != nullptr)
                {
                    freeaddrinfo(AddressInfoResults);
                }

                // Move resources
                Sock = other.Sock;
                Hints = other.Hints;
                AddressInfoResults = other.AddressInfoResults;
                LastWinsockError = other.LastWinsockError;
                IpAddress = std::move(other.IpAddress);
                Port = std::move(other.Port);

                // Reset 'other' to a safe state
                other.Sock = INVALID_SOCKET;
                other.AddressInfoResults = nullptr;
            }

            return *this;
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
                else
                {
                    WinsockInitializerCls::IncrementInstanceCount();
                }
            }

            return result;
        }
        WinsockError SocketCommonCls::CleanupWinsock()
        {
            WinsockError result = WinsockError::Success;

            if (WinsockInitializerCls::GetInstanceCount() > 0)
            {
                int iResult = WSACleanup();
                if (iResult == SOCKET_ERROR)
                {
                    LastWinsockError = WSAGetLastError();
                    result = WinsockError::CheckLastWinsockError;
                }
                else
                {
                    WinsockInitializerCls::DecrementInstanceCount();
                }
            }

            return result;
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


        UdpCommonCls::UdpCommonCls() :
            SocketCommonCls()
        {
        }
        std::variant<WinsockError, UdpCommonCls> UdpCommonCls::Initialize(
            const addrinfo& hints, 
            const std::string& ipAddress, 
            const std::string& port, 
            BlockingMode blockingMode)
        {
            UdpCommonCls udp;

            udp.SetHints(hints);
            if (udp.SetIpAddress(ipAddress) == false)
            {
                return WinsockError::InvalidIpAddress;
            }
            if (udp.SetPort(port) == false)
            {
                return WinsockError::InvalidPort;
            }

            WinsockError result = udp.InitializeWinsock();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = udp.GetAddressInfo();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = udp.CreateSocket();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = udp.SetBlockingMode(blockingMode);
            if (result != WinsockError::Success)
            {
                return result;
            }

            return udp;
        }
        UdpCommonCls::UdpCommonCls(UdpCommonCls&& other) noexcept :
            SocketCommonCls(std::move(other))
        {
        }
        UdpCommonCls& UdpCommonCls::operator=(UdpCommonCls&& other) noexcept
        {
            SocketCommonCls::operator=(std::move(other));
            return *this;
        }
        WinsockError UdpCommonCls::RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount)
        {
            if (bufferLen > INT_MAX)
            {
                return WinsockError::BufferTooLong;
            }

            if (AddressInfoResults == nullptr ||
                Sock == INVALID_SOCKET)
            {
                return WinsockError::NotInitialized;
            }

            char* bufPtr = new (std::nothrow) char[bufferLen];
            if (bufPtr == nullptr)
            {
                return WinsockError::OutOfMemory;
            }

            int bufLen = static_cast<int>(bufferLen);
            sockaddr* addrPtr = reinterpret_cast<sockaddr*>(AddressInfoResults->ai_addr);
            int addrLen = static_cast<int>(sizeof(*addrPtr));

            int iResult = recvfrom(Sock, bufPtr, bufLen, 0, addrPtr, &addrLen);
            if (iResult == SOCKET_ERROR)
            {
                LastWinsockError = WSAGetLastError();
                delete[] bufPtr;
                return WinsockError::CheckLastWinsockError;
            }

            recvByteCount = static_cast<size_t>(iResult);
            buffer.assign(bufPtr, recvByteCount);

            delete[] bufPtr;
            return WinsockError::Success;
        }
        WinsockError UdpCommonCls::SendTo(const std::string& buffer, size_t bufferLen, size_t& sentByteCount)
        {
            sentByteCount = 0;
            if (bufferLen > INT_MAX)
            {
                return WinsockError::BufferTooLong;
            }
            if (AddressInfoResults == nullptr ||
                Sock == INVALID_SOCKET)
            {
                return WinsockError::NotInitialized;
            }

            const char* bufPtr = buffer.c_str();
            int bufLen = static_cast<int>(bufferLen);
            sockaddr* addrPtr = reinterpret_cast<sockaddr*>(AddressInfoResults->ai_addr);
            int addrLen = static_cast<int>(sizeof(*addrPtr));

            int iResult = sendto(Sock, buffer.c_str(), bufLen, 0, addrPtr, addrLen);
            if (iResult == SOCKET_ERROR)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }
            
            sentByteCount = static_cast<size_t>(iResult);
            return WinsockError::Success;
        }


        UdpServerCls::UdpServerCls() :
            UdpCommonCls()
        {
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

            UdpServerCls udpServer = std::move(std::get<UdpCommonCls>(initResult));

            WinsockError result = udpServer.Bind();

            if (result != WinsockError::Success)
            {
                return result;
            }

            return udpServer;
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
    }; // namespace Network
}; // namespace UtilityLib