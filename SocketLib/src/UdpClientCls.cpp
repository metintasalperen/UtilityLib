#include "UdpClientCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        UdpClientCls::UdpClientCls() :
            Sock(INVALID_SOCKET),
            LastWinsockError(0)
        {
            memset(Buffer, 0, sizeof(Buffer));
        }
        UdpClientCls::UdpClientCls(UdpClientCls&& other) noexcept :
            Sock(other.Sock),
            LastWinsockError(other.LastWinsockError),
            IpAddress(std::move(other.IpAddress)),
            Port(std::move(other.Port))
        {
            other.Sock = INVALID_SOCKET;
            memcpy(Buffer, other.Buffer, sizeof(Buffer));
        }
        UdpClientCls& UdpClientCls::operator=(UdpClientCls&& other) noexcept
        {
            if (this != &other)
            {
                Sock = other.Sock;
                LastWinsockError = other.LastWinsockError;
                IpAddress = std::move(other.IpAddress);
                Port = std::move(other.Port);
                memcpy(Buffer, other.Buffer, sizeof(Buffer));

                other.Sock = INVALID_SOCKET;
            }
            return *this;
        }
        UdpClientCls::~UdpClientCls()
        {
            CloseSocket();
        }

        bool UdpClientCls::SetIpAddress(const std::string& ipAddress)
        {
            if (UtilityLib::String::ValidateIpAddress(ipAddress))
            {
                IpAddress = ipAddress;
                return true;
            }
            return false;
        }
        bool UdpClientCls::SetPort(const std::string& port)
        {
            if (UtilityLib::String::ValidatePort(port))
            {
                Port = port;
                return true;
            }
            return false;
        }
        int UdpClientCls::GetLastWinsockError()
        {
            return LastWinsockError;
        }

        WinsockError UdpClientCls::CreateSocket()
        {
            Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (Sock == INVALID_SOCKET)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }
            return WinsockError::Success;
        }
        WinsockError UdpClientCls::CloseSocket()
        {
            if (Sock != INVALID_SOCKET)
            {
                int iResult = closesocket(Sock);
                if (iResult == SOCKET_ERROR)
                {
                    LastWinsockError = WSAGetLastError();
                    return WinsockError::CheckLastWinsockError;
                }
                else
                {
                    Sock = INVALID_SOCKET;
                    return WinsockError::Success;
                }
            }

            return WinsockError::Success;
        }
        WinsockError UdpClientCls::SetBlockingMode(BlockingMode mode)
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

        std::variant<WinsockError, UdpClientCls> UdpClientCls::Initialize(const std::string& ipAddress, const std::string& port, BlockingMode mode)
        {
            UdpClientCls udp;

            if (udp.SetIpAddress(ipAddress) == false)
            {
                return WinsockError::InvalidIpAddress;
            }
            if (udp.SetPort(port) == false)
            {
                return WinsockError::InvalidPort;
            }

            WinsockError result = udp.CreateSocket();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = udp.SetBlockingMode(mode);
            if (result != WinsockError::Success)
            {
                return result;
            }

            return udp;
        }

        WinsockError UdpClientCls::RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount)
        {
            if (bufferLen > INT_MAX) return WinsockError::BufferTooLong;
            if (bufferLen == 0) return WinsockError::BufferLengthIsZero;
            if (Sock == INVALID_SOCKET) return WinsockError::NotInitialized;

            char* bufPtr = Buffer;
            if (bufferLen > 2048)
            {
                bufPtr = new (std::nothrow) char[bufferLen];
                if (bufPtr == nullptr) return WinsockError::OutOfMemory;
            }

            int bufLen = static_cast<int>(bufferLen);
            sockaddr addr{ 0 };
            int addrLen = sizeof(addr);

            int iResult = recvfrom(Sock, bufPtr, bufLen, 0, &addr, &addrLen);
            if (iResult == SOCKET_ERROR)
            {
                if (bufferLen > 2048) delete[] bufPtr;
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }

            recvByteCount = static_cast<size_t>(iResult);
            buffer.assign(bufPtr, recvByteCount);
            if (bufferLen > 2048) delete[] bufPtr;
            return WinsockError::Success;
        }
        WinsockError UdpClientCls::RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount, std::string& fromIpAddr, std::string& fromPort)
        {
            if (bufferLen > INT_MAX) return WinsockError::BufferTooLong;
            if (bufferLen == 0) return WinsockError::BufferLengthIsZero;
            if (Sock == INVALID_SOCKET) return WinsockError::NotInitialized;

            char* bufPtr = Buffer;
            if (bufferLen > 2048)
            {
                bufPtr = new (std::nothrow) char[bufferLen];
                if (bufPtr == nullptr) return WinsockError::OutOfMemory;
            }

            int bufLen = static_cast<int>(bufferLen);
            sockaddr addr{ 0 };
            int addrLen = sizeof(addr);

            int iResult = recvfrom(Sock, bufPtr, bufLen, 0, &addr, &addrLen);
            if (iResult == SOCKET_ERROR)
            {
                if (bufferLen > 2048) delete[] bufPtr;
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }

            recvByteCount = static_cast<size_t>(iResult);
            buffer.assign(bufPtr, recvByteCount);
            if (bufferLen > 2048) delete[] bufPtr;

            fromIpAddr = SockaddrInToIpAddress(reinterpret_cast<sockaddr_in*>(&addr));
            fromPort = UtilityLib::String::IntegralToString(ntohs(reinterpret_cast<sockaddr_in*>(&addr)->sin_port));

            return WinsockError::Success;
        }
        WinsockError UdpClientCls::SendTo(const std::string& buffer, size_t bufferLen, size_t& sentByteCount)
        {
            if (bufferLen > INT_MAX) return WinsockError::BufferTooLong;
            if (bufferLen == 0) return WinsockError::BufferLengthIsZero;
            if (Sock == INVALID_SOCKET) return WinsockError::NotInitialized;

            const char* bufPtr = buffer.c_str();
            int bufLen = static_cast<int>(bufferLen);
            sockaddr_in addr = CreateSockaddrIn(Port, IpAddress);
            sockaddr* addrPtr = reinterpret_cast<sockaddr*>(&addr);
            int addrLen = sizeof(*addrPtr);

            int iResult = sendto(Sock, bufPtr, bufLen, 0, addrPtr, addrLen);
            if (iResult == SOCKET_ERROR)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }

            sentByteCount = static_cast<size_t>(iResult);
            return WinsockError::Success;
        }
    }
}