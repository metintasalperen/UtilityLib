#include "SocketCls.h"

namespace UtilityLib
{
    namespace Network
    {
        SocketCommonCls::SocketCommonCls() :
            Sock(INVALID_SOCKET),
            Hints(),
            WSAErrorCls()
        {
        }
        SocketCommonCls::SocketCommonCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port) :
            Sock(INVALID_SOCKET),
            Hints(hints),
            IpAddress(ipAddress),
            Port(port),
            WSAErrorCls()
        {
        }
        SocketCommonCls::~SocketCommonCls()
        {
            CloseSocket();
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

                buffer.assign(buf, iResult);
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
        ErrorEnum SocketClientCls::GetAddressInfo()
        {
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
        ErrorEnum SocketClientCls::GetAddressInfo(const addrinfo& hints, const std::string& ipAddress, const std::string& port)
        {
            Hints = hints;
            IpAddress = ipAddress;
            Port = port;

            return GetAddressInfo();
        }
        ErrorEnum SocketClientCls::Connect()
        {
            if (AddressInfoResults == nullptr)
            {
                return ErrorEnum::WinsockCallGetAddressInfo;
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