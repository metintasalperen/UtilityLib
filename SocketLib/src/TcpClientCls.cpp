#include "TcpClientCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        TcpClientCls::TcpClientCls() :
            Sock(INVALID_SOCKET),
            LastWinsockError(0),
            AddressInfoResults(nullptr),
            Hints{0},
            Buffer{0}
        {
        }
        TcpClientCls::TcpClientCls(TcpClientCls&& other) noexcept :
            Sock(other.Sock),
            LastWinsockError(other.LastWinsockError),
            IpAddress(std::move(other.IpAddress)),
            Port(std::move(other.Port)),
            Hints(other.Hints),
            AddressInfoResults(other.AddressInfoResults)
        {
            memcpy(Buffer, other.Buffer, sizeof(Buffer));

            other.Sock = INVALID_SOCKET;
            other.AddressInfoResults = nullptr;
        }
        TcpClientCls& TcpClientCls::operator=(TcpClientCls&& other) noexcept
        {
            if (this != &other)
            {
                Sock = other.Sock;
                LastWinsockError = other.LastWinsockError;
                IpAddress = std::move(other.IpAddress);
                Port = std::move(other.Port);
                Hints = other.Hints;
                AddressInfoResults = other.AddressInfoResults;
                memcpy(Buffer, other.Buffer, sizeof(Buffer));

                other.Sock = INVALID_SOCKET;
                other.AddressInfoResults = nullptr;
            }
            return *this;
        }
        TcpClientCls::~TcpClientCls()
        {
            if (AddressInfoResults != nullptr)
            {
                freeaddrinfo(AddressInfoResults);
            }

            CloseSocket();
        }

        bool TcpClientCls::SetIpAddress(const std::string& ipAddress)
        {
            if (UtilityLib::String::ValidateIpAddress(ipAddress))
            {
                IpAddress = ipAddress;
                return true;
            }
            return false;
        }
        bool TcpClientCls::SetPort(const std::string& port)
        {
            if (UtilityLib::String::ValidatePort(port))
            {
                Port = port;
                return true;
            }
            return false;
        }
        int TcpClientCls::GetLastWinsockError()
        {
            return LastWinsockError;
        }

        WinsockError TcpClientCls::GetAddressInfo()
        {
            WinsockError result = WinsockError::Success;

            if (AddressInfoResults != nullptr)
            {
                freeaddrinfo(AddressInfoResults);
                AddressInfoResults = nullptr;
            }

            int iResult = getaddrinfo(IpAddress.c_str(), Port.c_str(), &Hints, &AddressInfoResults);
            if (iResult == SOCKET_ERROR)
            {
                LastWinsockError = WSAGetLastError();
                result = WinsockError::CheckLastWinsockError;
            }

            return result;
        }
        WinsockError TcpClientCls::CreateSocket()
        {
            Sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (Sock == INVALID_SOCKET)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }
            return WinsockError::Success;
        }
        WinsockError TcpClientCls::CloseSocket()
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

            LastWinsockError = WSAGetLastError();
            return WinsockError::CheckLastWinsockError;
        }
        WinsockError TcpClientCls::SetBlockingMode(BlockingMode mode)
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

        std::variant<WinsockError, TcpClientCls> TcpClientCls::Initialize(const addrinfo& hints, const std::string& ipAddress, const std::string& port, BlockingMode mode)
        {
            TcpClientCls tcp;

            if (tcp.SetIpAddress(ipAddress) == false)
            {
                return WinsockError::InvalidIpAddress;
            }
            if (tcp.SetPort(port) == false)
            {
                return WinsockError::InvalidPort;
            }

            tcp.Hints = hints;
            WinsockError result = tcp.GetAddressInfo();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = tcp.CreateSocket();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = tcp.Connect();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = tcp.SetBlockingMode(mode);
            if (result != WinsockError::Success)
            {
                return result;
            }

            return tcp;
        }

        WinsockError TcpClientCls::Recv(std::string& buffer, size_t bufferLen, size_t& recvByteCount)
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

            int iResult = recv(Sock, bufPtr, bufLen, 0);
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
        WinsockError TcpClientCls::Send(const std::string& buffer, size_t bufferLen, size_t& sentByteCount)
        {
            if (bufferLen > INT_MAX) return WinsockError::BufferTooLong;
            if (bufferLen == 0) return WinsockError::BufferLengthIsZero;
            if (Sock == INVALID_SOCKET) return WinsockError::NotInitialized;

            sentByteCount = 0;
            const char* bufPtr = buffer.c_str();
            int bufLen = static_cast<int>(bufferLen);

            int iResult = send(Sock, bufPtr, bufLen, 0);
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