#include "TcpServerCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        TcpServerCls::TcpServerCls() :
            Sock(INVALID_SOCKET),
            LastWinsockError(0)
        {
        }
        TcpServerCls::TcpServerCls(TcpServerCls&& other) noexcept :
            Sock(other.Sock),
            LastWinsockError(other.LastWinsockError),
            Port(std::move(other.Port))
        {
            other.Sock = INVALID_SOCKET;
        }
        TcpServerCls& TcpServerCls::operator=(TcpServerCls&& other) noexcept
        {
            if (this != &other)
            {
                Sock = other.Sock;
                LastWinsockError = other.LastWinsockError;
                Port = std::move(other.Port);

                other.Sock = INVALID_SOCKET;
            }
            return *this;
        }
        TcpServerCls::~TcpServerCls()
        {
            CloseSocket();
        }

        bool TcpServerCls::SetPort(const std::string& port)
        {
            if (UtilityLib::String::ValidatePort(port))
            {
                Port = port;
                return true;
            }
            return false;
        }
        int TcpServerCls::GetLastWinsockError()
        {
            return LastWinsockError;
        }

        WinsockError TcpServerCls::CreateSocket()
        {
            Sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (Sock == INVALID_SOCKET)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }
            return WinsockError::Success;
        }
        WinsockError TcpServerCls::CloseSocket()
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
        WinsockError TcpServerCls::SetBlockingMode(BlockingMode mode)
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
        WinsockError TcpServerCls::Bind()
        {
            if (Sock == INVALID_SOCKET) return WinsockError::NotInitialized;

            sockaddr_in addr = StringToSockaddrIn(Port);
            int iResult = bind(Sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr));

            if (iResult == SOCKET_ERROR)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }

            return WinsockError::Success;
        }
        WinsockError TcpServerCls::Listen(int backlog)
        {
            if (Sock == INVALID_SOCKET) return WinsockError::NotInitialized;

            int iResult = listen(Sock, backlog);
            if (iResult == SOCKET_ERROR)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }

            return WinsockError::Success;
        }
        std::variant<WinsockError, TcpSessionCls> TcpServerCls::Accept()
        {
            sockaddr addr;
            int addrLen = sizeof(addr);
            SOCKET sock = accept(Sock, &addr, &addrLen);

            if (sock == INVALID_SOCKET)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }

            sockaddr_in* addrIn = reinterpret_cast<sockaddr_in*>(&addr);
            std::string clientIpAddr = SockaddrInToString(addrIn);
            std::string clientPort = UtilityLib::String::IntegralToString<USHORT>(ntohs(addrIn->sin_port));

            TcpSessionCls tcpSession(sock, clientIpAddr, clientPort);
            return tcpSession;
        }

        std::variant<WinsockError, TcpServerCls> TcpServerCls::Initialize(const std::string& port, BlockingMode mode, int backlog)
        {
            TcpServerCls tcp;

            if (tcp.SetPort(port) == false)
            {
                return WinsockError::InvalidPort;
            }

            WinsockError result = tcp.CreateSocket();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = tcp.Bind();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = tcp.Listen(backlog);
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
    }
}