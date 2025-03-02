#include "SocketPkg.h"

namespace UtilityLib
{
    namespace Network
    {
        SocketCommonCls::SocketCommonCls() :
            WSAErrorCls(),
            Sock(INVALID_SOCKET),
            Hints{},
            AddressInfoResults(nullptr)
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
        ErrorEnum SocketCommonCls::GetAddressInfo()
        {
            // Before getting new address info, cleanup already acquired address info first..
            if (AddressInfoResults != nullptr)
            {
                freeaddrinfo(AddressInfoResults);
            }

            int iResult = getaddrinfo(IpAddress.c_str(),
                Port.c_str(),
                &Hints,
                &AddressInfoResults);

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
        ErrorEnum SocketCommonCls::CreateSocket()
        {
            ErrorEnum result = ErrorEnum::Success;
            Sock = socket(Hints.ai_family, Hints.ai_socktype, Hints.ai_protocol);

            if (Sock == INVALID_SOCKET)
            {
                result = ErrorEnum::WinsockError;
                SetLastWsaError(WSAGetLastError());
            }

            return result;
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
        ErrorEnum SocketCommonCls::Recv(std::string& buffer, size_t bufferLength)
        {
            if (bufferLength > INT_MAX)
            {
                return ErrorEnum::InvalidArgument;
            }

            char* buf = new char[bufferLength];
            buffer.clear();

            if (buf != nullptr)
            {
                int iResult = recv(Sock, buf, static_cast<int>(bufferLength), 0);

                if (iResult < 0)
                {
                    delete[] buf;
                    SetLastWsaError(WSAGetLastError());
                    return ErrorEnum::WinsockError;
                }
                if (iResult == 0)
                {
                    delete[] buf;
                    return ErrorEnum::WinsockConnClosed;
                }

                buffer.assign(buf, static_cast<size_t>(iResult));
                delete[] buf;
                return ErrorEnum::Success;
            }
            else
            {
                return ErrorEnum::InsufficientMemory;
            }
        }
        ErrorEnum SocketCommonCls::Send(const std::string& buffer, size_t& bufferLength)
        {
            if (buffer.size() > INT_MAX)
            {
                return ErrorEnum::OutOfRange;
            }

            int iResult = send(Sock, buffer.c_str(), static_cast<int>(buffer.size()), 0);

            if (iResult < 0)
            {
                SetLastWsaError(WSAGetLastError());
                bufferLength = 0;
                return ErrorEnum::WinsockError;
            }
            else if (iResult == 0)
            {
                bufferLength = 0;
                return ErrorEnum::WinsockConnClosed;
            }

            bufferLength = static_cast<size_t>(iResult);
            return ErrorEnum::Success;
        }
        ErrorEnum SocketCommonCls::SetBlockingMode(BlockingMode mode)
        {
            u_long blockingMode = static_cast<u_long>(mode);
            int iResult = ioctlsocket(Sock, FIONBIO, &blockingMode);

            if (iResult != 0)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }

            return ErrorEnum::Success;
        }

        SocketServerCls::SocketServerCls() :
            SocketCommonCls()
        {
        }
        SocketServerCls::SocketServerCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port)
            : SocketCommonCls(hints, ipAddress, port)
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
            SOCKET ClientSock = accept(Sock, nullptr, nullptr);

            if (ClientSock == INVALID_SOCKET)
            {
                SetLastWsaError(WSAGetLastError());
                return ErrorEnum::WinsockError;
            }

            clientSockets.push_back(ClientSock);
            return ErrorEnum::Success;
        }

        SocketClientCls::SocketClientCls() :
            SocketCommonCls(),
            AddressInfoResults(nullptr)
        {
        }
        SocketClientCls::SocketClientCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port) :
            SocketCommonCls(hints, ipAddress, port),
            AddressInfoResults(nullptr)
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
    };
};