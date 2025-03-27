#include "UdpServerCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        UdpServerCls::UdpServerCls() :
            Sock(INVALID_SOCKET),
            LastWinsockError(0)
        {
            memset(Buffer, 0, sizeof(Buffer));
        }
        UdpServerCls::UdpServerCls(UdpServerCls&& other) noexcept :
            Sock(other.Sock),
            LastWinsockError(other.LastWinsockError),
            Port(std::move(other.Port))
        {
            memcpy(Buffer, other.Buffer, sizeof(Buffer));

            other.Sock = INVALID_SOCKET;
        }
        UdpServerCls& UdpServerCls::operator=(UdpServerCls&& other) noexcept
        {
            if (this != &other)
            {
                Sock = other.Sock;
                LastWinsockError = other.LastWinsockError;
                Port = std::move(other.Port);
                memcpy(Buffer, other.Buffer, sizeof(Buffer));

                other.Sock = INVALID_SOCKET;
            }
            return *this;
        }
        UdpServerCls::~UdpServerCls()
        {
            CloseSocket();
        }
        int UdpServerCls::GetLastWinsockError()
        {
            return LastWinsockError;
        }

        std::variant<WinsockError, UdpServerCls> UdpServerCls::Initialize(const std::string& port, BlockingMode blockingMode, const std::string& ipAddress)
        {
            if (ipAddress != "" && UtilityLib::String::ValidateIpAddress(ipAddress) == false)
            {
                return WinsockError::InvalidIpAddress;
            }

            UdpServerCls udp;
            if (udp.SetPort(port) == false)
            {
                return WinsockError::InvalidPort;
            }

            WinsockError result = udp.CreateSocket();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = udp.SetBlockingMode(blockingMode);
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = udp.Bind(ipAddress);
            if (result != WinsockError::Success)
            {
                return result;
            }

            return udp;
        }

        bool UdpServerCls::SetPort(const std::string& port)
        {
            if (UtilityLib::String::ValidatePort(port))
            {
                Port = port;
                return true;
            }
            return false;
        }

        WinsockError UdpServerCls::CreateSocket()
        {
            Sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (Sock == INVALID_SOCKET)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }
            return WinsockError::Success;
        }
        WinsockError UdpServerCls::CloseSocket()
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
        WinsockError UdpServerCls::SetBlockingMode(BlockingMode mode)
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
        WinsockError UdpServerCls::Bind(const std::string& ipAddress)
        {
            if (Sock == INVALID_SOCKET) return WinsockError::NotInitialized;

            struct sockaddr_in addr = StringToSockaddrIn(Port, ipAddress);

            int iResult = bind(Sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
            if (iResult != SOCKET_ERROR)
            {
                return WinsockError::Success;
            }

            LastWinsockError = WSAGetLastError();
            return WinsockError::CheckLastWinsockError;
        }

        WinsockError UdpServerCls::RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount, std::string& fromIpAddr, std::string& fromPort)
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

            sockaddr_in* ptr = reinterpret_cast<sockaddr_in*>(&addr);
            fromIpAddr = SockaddrInToString(ptr);
            fromPort = String::IntegralToString<USHORT>(ntohs(ptr->sin_port));

            return WinsockError::Success;
        }
        WinsockError UdpServerCls::SendTo(const std::string& buffer, size_t bufferLen, size_t& sentByteCount, const std::string& toIpAddr, const std::string& toPort)
        {
            sentByteCount = 0;

            if (bufferLen > INT_MAX) return WinsockError::BufferTooLong;
            if (bufferLen == 0) return WinsockError::BufferLengthIsZero;
            if (Sock == INVALID_SOCKET) return WinsockError::NotInitialized;

            const char* bufPtr = buffer.c_str();
            int bufLen = static_cast<int>(bufferLen);
            sockaddr_in addr = StringToSockaddrIn(toPort, toIpAddr);
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