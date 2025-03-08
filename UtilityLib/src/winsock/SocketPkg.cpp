#include "SocketPkg.h"

namespace UtilityLib
{
    namespace Network
    {
        SocketCommonCls::SocketCommonCls() :
            WSAErrorCls(),
            Sock(INVALID_SOCKET),
            AddressInfoResults(nullptr),
            Hints{}
        {
        }
        SocketCommonCls::SocketCommonCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port) : 
            WSAErrorCls(), 
            Sock(INVALID_SOCKET),
            Hints(hints),
            IpAddress(ipAddress),
            Port(port),
            AddressInfoResults(nullptr)
        {
        }
        SocketCommonCls::~SocketCommonCls()
        {
            if (AddressInfoResults != nullptr)
            {
                freeaddrinfo(AddressInfoResults);
            }

            if (Sock != INVALID_SOCKET)
            {
                CloseSocket();
            }
            
            CleanupWinsock();
        }
        ErrorEnum SocketCommonCls::InitializeWinsock()
        {
            ErrorEnum result = ErrorEnum::Success;

            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

            if (iResult != 0)
            {
                SetLastWsaError(WSAGetLastError());
                result = ErrorEnum::WinsockError;
            }

            return result;
        }
        ErrorEnum SocketCommonCls::CleanupWinsock()
        {
            ErrorEnum result = ErrorEnum::Success;
            
            int iResult = WSACleanup();
            if (iResult == SOCKET_ERROR)
            {
                SetLastWsaError(WSAGetLastError());
                result = ErrorEnum::WinsockError;
            }

            return result;
        }
        void SocketCommonCls::SetHints(const addrinfo& hints)
        {
            Hints = hints;
        }
        void SocketCommonCls::SetIpAddress(const std::string& ipAddress)
        {
            IpAddress = ipAddress;
        }
        void SocketCommonCls::SetPort(const std::string& port)
        {
            Port = port;
        }
        ErrorEnum SocketCommonCls::CreateSocket()
        {
            if (AddressInfoResults == nullptr)
            {
                return ErrorEnum::WinsockCallGetAddressInfo;
            }

            for (const addrinfo* ptr = AddressInfoResults; ptr != nullptr; ptr = ptr->ai_next)
            {
                Sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

                if (Sock != INVALID_SOCKET)
                {
                    return ErrorEnum::Success;
                }
            }

            SetLastWsaError(WSAGetLastError());
            return ErrorEnum::WinsockError;
        }
        ErrorEnum SocketCommonCls::CloseSocket()
        {
            ErrorEnum result = ErrorEnum::Success;
            int iResult = closesocket(Sock);

            if (iResult != 0)
            {
                SetLastWsaError(WSAGetLastError());
                result = ErrorEnum::WinsockError;
            }
            else
            {
                Sock = INVALID_SOCKET;
            }

            return result;
        }
        ErrorEnum SocketCommonCls::GetAddressInfo()
        {
            if (AddressInfoResults != nullptr)
            {
                freeaddrinfo(AddressInfoResults);
            }

            int iResult = getaddrinfo(IpAddress.c_str(), Port.c_str(), &Hints, &AddressInfoResults);

            if (iResult != 0)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }
            return ErrorEnum::Success;
        }
        ErrorEnum SocketCommonCls::GetAddressInfo(const addrinfo& hints, const std::string& ipAddress, const std::string& port)
        {
            Hints = hints;
            IpAddress = ipAddress;
            Port = port;

            return GetAddressInfo();
        }
        ErrorEnum SocketCommonCls::Recv(std::string& buffer, size_t bufferLength)
        {
            buffer.clear();

            if (bufferLength > INT_MAX)
            {
                return ErrorEnum::OutOfRange;
            }

            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            std::string buf(bufferLength, '\0');
            int iResult = recv(Sock, buf.data(), static_cast<int>(bufferLength), 0);

            if (iResult < 0)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }
            if (iResult == 0)
            {
                return ErrorEnum::WinsockConnClosed;
            }

            buffer.assign(buf, static_cast<size_t>(iResult));
            return ErrorEnum::Success;
        }
        ErrorEnum SocketCommonCls::Send(const std::string& buffer, size_t& bytesSent)
        {
            if (buffer.size() > INT_MAX)
            {
                return ErrorEnum::OutOfRange;
            }

            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            int iResult = send(Sock, buffer.c_str(), static_cast<int>(buffer.size()), 0);

            if (iResult < 0)
            {
                SetLastWsaError(WSAGetLastError());
                bytesSent = 0;
                return ErrorEnum::WinsockError;
            }
            if (iResult == 0)
            {
                bytesSent = 0;
                return ErrorEnum::WinsockConnClosed;
            }

            bytesSent = static_cast<size_t>(iResult);
            return ErrorEnum::Success;
        }
        ErrorEnum SocketCommonCls::SetBlockingMode(BlockingMode mode)
        {
            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            u_long blockingMode = static_cast<u_long>(mode);
            int iResult = ioctlsocket(Sock, FIONBIO, &blockingMode);

            if (iResult != 0)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }
            return ErrorEnum::Success;
        }


        SocketClientCls::SocketClientCls() :
            SocketCommonCls()
        {
        }
        SocketClientCls::SocketClientCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port) :
            SocketCommonCls(hints, ipAddress, port)
        {
        }
        ErrorEnum SocketClientCls::Connect()
        {
            if (AddressInfoResults == nullptr)
            {
                return ErrorEnum::WinsockCallGetAddressInfo;
            }

            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            for (const addrinfo* ptr = AddressInfoResults; ptr != nullptr; ptr = ptr->ai_next)
            {
                int iResult = connect(Sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));

                if (iResult != SOCKET_ERROR)
                {
                    return ErrorEnum::Success;
                }
            }

            SetLastWsaError(WSAGetLastError());
            return ErrorEnum::WinsockError;
        }
        ErrorEnum SocketClientCls::SendTo(const std::string& buffer, size_t& bytesSent)
        {
            bytesSent = 0;
            if (buffer.size() > INT_MAX)
            {
                return ErrorEnum::OutOfRange;
            }

            if (AddressInfoResults == nullptr)
            {
                return ErrorEnum::WinsockCallGetAddressInfo;
            }

            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            const char* bufPtr = buffer.c_str();
            int bufLen = static_cast<int>(buffer.size());
            const sockaddr* addrPtr = reinterpret_cast<const sockaddr*>(AddressInfoResults->ai_addr);
            int addrLen = static_cast<int>(sizeof(*addrPtr));

            int iResult = sendto(Sock, bufPtr, bufLen, 0, addrPtr, addrLen);
            if (iResult == SOCKET_ERROR)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }

            bytesSent = static_cast<size_t>(iResult);
            return ErrorEnum::Success;
        }
        ErrorEnum SocketClientCls::RecvFrom(std::string& buffer, size_t bufferLength)
        {
            buffer.clear();

            if (bufferLength > INT_MAX)
            {
                return ErrorEnum::OutOfRange;
            }

            if (AddressInfoResults == nullptr)
            {
                return ErrorEnum::WinsockCallGetAddressInfo;
            }

            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            std::string buf(bufferLength, '\0');
            char* bufPtr = buf.data();
            int bufLen = static_cast<int>(bufferLength);
            sockaddr* addrPtr = reinterpret_cast<sockaddr*>(AddressInfoResults->ai_addr);
            int addrLen = static_cast<int>(sizeof(*addrPtr));

            int iResult = recvfrom(Sock, bufPtr, bufLen, 0, addrPtr, &addrLen);
            if (iResult == SOCKET_ERROR)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }

            buffer.assign(buf.c_str(), static_cast<size_t>(iResult));
            return ErrorEnum::Success;
        }
        ErrorEnum SocketClientCls::SendTo(const char* buffer, size_t bufferLength, size_t& bytesSent)
        {
            bytesSent = 0;
            if (bufferLength > INT_MAX)
            {
                return ErrorEnum::OutOfRange;
            }

            if (AddressInfoResults == nullptr)
            {
                return ErrorEnum::WinsockCallGetAddressInfo;
            }

            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            const sockaddr* addrPtr = reinterpret_cast<const sockaddr*>(AddressInfoResults->ai_addr);
            int addrLen = static_cast<int>(sizeof(*addrPtr));

            int iResult = sendto(Sock, buffer, static_cast<int>(bufferLength), 0, addrPtr, addrLen);
            if (iResult == SOCKET_ERROR)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }

            bytesSent = static_cast<size_t>(iResult);
            return ErrorEnum::Success;
        }


        SocketServerCls::SocketServerCls() :
            SocketCommonCls(),
            RemoteAddress{}
        {
        }
        SocketServerCls::SocketServerCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port) :
            SocketCommonCls(hints, ipAddress, port),
            RemoteAddress{}
        {
        }
        ErrorEnum SocketServerCls::Bind()
        {
            if (AddressInfoResults == nullptr)
            {
                return ErrorEnum::WinsockCallGetAddressInfo;
            }
            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            for (const addrinfo* ptr = AddressInfoResults; ptr != nullptr; ptr = ptr->ai_next)
            {
                int iResult = bind(Sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));

                if (iResult != SOCKET_ERROR)
                {
                    return ErrorEnum::Success;
                }
            }

            SetLastWsaError(WSAGetLastError());
            return ErrorEnum::WinsockError;
        }
        ErrorEnum SocketServerCls::Listen()
        {
            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            int iResult = listen(Sock, SOMAXCONN);

            if (iResult == SOCKET_ERROR)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }
            return ErrorEnum::Success;
        }
        ErrorEnum SocketServerCls::Accept()
        {
            SOCKET clientSock = accept(Sock, nullptr, nullptr);

            if (clientSock == INVALID_SOCKET)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }
            return ErrorEnum::Success;
        }
        ErrorEnum SocketServerCls::SendTo(const std::string& buffer, size_t& bytesSent)
        {
            bytesSent = 0;
            if (buffer.size() > INT_MAX)
            {
                return ErrorEnum::OutOfRange;
            }

            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            const char* bufPtr = buffer.c_str();
            int bufLen = static_cast<int>(buffer.size());
            const sockaddr* addrPtr = reinterpret_cast<const sockaddr*>(&RemoteAddress);
            int addrLen = static_cast<int>(sizeof(RemoteAddress));

            int iResult = sendto(Sock, bufPtr, bufLen, 0, addrPtr, addrLen);
            if (iResult == SOCKET_ERROR)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }

            bytesSent = static_cast<size_t>(iResult);
            return ErrorEnum::Success;
        }
        ErrorEnum SocketServerCls::RecvFrom(std::string& buffer, size_t bufferLength)
        {
            buffer.clear();

            if (bufferLength > INT_MAX)
            {
                return ErrorEnum::OutOfRange;
            }

            if (Sock == INVALID_SOCKET)
            {
                return ErrorEnum::WinsockCallCreateSocket;
            }

            std::string buf(bufferLength, '\0');
            char* bufPtr = buf.data();
            int bufLen = static_cast<int>(bufferLength);
            sockaddr* addrPtr = reinterpret_cast<sockaddr*>(&RemoteAddress);
            int addrLen = static_cast<int>(sizeof(RemoteAddress));

            int iResult = recvfrom(Sock, bufPtr, bufLen, 0, addrPtr, &addrLen);
            if (iResult == SOCKET_ERROR)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }

            buffer.assign(buf.c_str(), static_cast<size_t>(iResult));
            return ErrorEnum::Success;
        }
    };
};