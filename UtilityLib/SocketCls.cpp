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
            char* buf = new char[bufferLength];
            buffer.clear();

            if (buf != nullptr)
            {
                int iResult = recv(Sock, buf, bufferLength, 0);

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

        SocketCls::SocketCls() :
            Sock(INVALID_SOCKET),
            AddressInfoResults(nullptr),
            Hints(),
            LastWsaError(0)
        {
        }
        SocketCls::SocketCls(const addrinfo& hints) :
            Sock(INVALID_SOCKET),
            AddressInfoResults(nullptr),
            Hints(hints),
            LastWsaError(0)
        {
        }
        SocketCls::SocketCls(const std::string& ipAddress, const std::string& port) :
            Sock(INVALID_SOCKET),
            AddressInfoResults(nullptr),
            Hints(),
            LastWsaError(0),
            IpAddress(ipAddress),
            Port(port)
        {
        }
        SocketCls::SocketCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port) :
            Sock(INVALID_SOCKET),
            AddressInfoResults(nullptr),
            Hints(hints),
            LastWsaError(0),
            IpAddress(ipAddress),
            Port(port)
        {
            bool result = InitializeWinsock();
            if (result == false)
            {
                return;
            }

            result = GetAddressInfo(IpAddress, Port);
            if (result == false)
            {
                CleanupWinsock();
                return;
            }

            result = CreateSocket();
            if (result == false)
            {
                CleanupWinsock();
                return;
            }
        }
        SocketCls::~SocketCls()
        {
            if (AddressInfoResults != nullptr)
            {
                freeaddrinfo(AddressInfoResults);
            }

            CloseSocket();
            CleanupWinsock();
        }

        bool SocketCls::InitializeWinsock()
        {
            bool result = true;
            WSADATA wsaData;
            int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

            if (iResult != 0)
            {
                LastWsaError = WSAGetLastError();
                result = false;
            }

            return result;
        }
        bool SocketCls::CleanupWinsock()
        {
            int iResult = WSACleanup();

            if (iResult == SOCKET_ERROR)
            {
                LastWsaError = WSAGetLastError();
                return false;
            }

            return true;
        }

        void SocketCls::SetHints(const addrinfo& hints)
        {
            Hints = hints;
        }
        addrinfo* SocketCls::GetAddressInfoResults()
        {
            return AddressInfoResults;
        }
        SOCKET SocketCls::GetSocket() const
        {
            return Sock;
        }
        int SocketCls::GetLastSocketError() const
        {
            return LastWsaError;
        }
        void SocketCls::SetLastSocketError(int errorCode)
        {
            LastWsaError = errorCode;
        }

        bool SocketCls::GetAddressInfo()
        {
            int iResult = getaddrinfo(IpAddress.c_str(),
                Port.c_str(),
                &Hints,
                &AddressInfoResults);

            if (iResult < 0)
            {
                LastWsaError = WSAGetLastError();
                return false;
            }
            return true;
        }
        bool SocketCls::GetAddressInfo(const addrinfo& hints)
        {
            Hints = hints;
            return GetAddressInfo();
        }
        bool SocketCls::GetAddressInfo(const std::string& ipAddress, const std::string& port)
        {
            IpAddress = ipAddress;
            Port = port;
            return GetAddressInfo();
        }
        bool SocketCls::GetAddressInfo(const addrinfo& hints, const std::string& ipAddress, const std::string& port)
        {
            Hints = hints;
            IpAddress = ipAddress;
            Port = port;
            return GetAddressInfo();
        }
        bool SocketCls::CreateSocket()
        {
            Sock = INVALID_SOCKET;

            for (const addrinfo* addrinfoPtr = AddressInfoResults;
                addrinfoPtr != nullptr && Sock == INVALID_SOCKET;
                addrinfoPtr = addrinfoPtr->ai_next)
            {
                Sock = socket(addrinfoPtr->ai_family, addrinfoPtr->ai_socktype, addrinfoPtr->ai_protocol);
            }

            if (Sock == INVALID_SOCKET)
            {
                LastWsaError = WSAGetLastError();
                return false;
            }
            return true;
        }
        bool SocketCls::CloseSocket()
        {
            int iResult = closesocket(Sock);
            if (iResult != 0)
            {
                LastWsaError = WSAGetLastError();
                return false;
            }

            Sock = INVALID_SOCKET;
            return true;
        }
        bool SocketCls::Connect()
        {
            for (const addrinfo* ptr = AddressInfoResults; ptr != nullptr; ptr = ptr->ai_next)
            {
                int iResult = connect(Sock, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
                if (iResult != SOCKET_ERROR)
                {
                    return true;
                }
            }

            LastWsaError = WSAGetLastError();
            return false;
        }
        bool SocketCls::Bind()
        {
            sockaddr_in serverAddr = { 0 };
            serverAddr.sin_family = Hints.ai_family;
            int port;
            ErrorEnum convResult = UtilityLib::String::StringToIntegral(Port, 0, Port.size(), port);
            if (convResult != ErrorEnum::Success)
            {
                return false;
            }
            serverAddr.sin_port = port;
            serverAddr.sin_addr.S_un.S_addr = inet_addr(IpAddress.c_str());

            sockaddr* serverPtr = reinterpret_cast<sockaddr*>(&serverAddr);
            int iResult = bind(Sock, serverPtr, sizeof(serverAddr));
            if (iResult == SOCKET_ERROR)
            {
                LastWsaError = WSAGetLastError();
                return false;
            }
            return true;
        }
        bool SocketCls::SetBlockingMode(BlockingMode mode)
        {
            u_long blockingMode = static_cast<u_long>(mode);
            int iResult = ioctlsocket(Sock, FIONBIO, &blockingMode);

            if (iResult != 0)
            {
                LastWsaError = WSAGetLastError();
                return false;
            }

            return true;
        }

        int SocketCls::Send(const std::string& buffer)
        {
            if (buffer.size() > INT_MAX)
            {
                LastWsaError = 0;
                return SOCKET_ERROR;
            }

            int iResult = send(Sock, buffer.c_str(), static_cast<int>(buffer.size()), 0);

            if (iResult == SOCKET_ERROR)
            {
                LastWsaError = WSAGetLastError();
            }

            return iResult;
        }
        int SocketCls::SendAll(const std::string& buffer)
        {
            size_t bufferSize = buffer.size();
            int sentBytes = 0;

            if (bufferSize <= static_cast<size_t>(INT_MAX))
            {
                std::string modifiableBuffer = buffer;
                while (sentBytes != bufferSize)
                {
                    // For warning of conversion between size_t and int...
                    int len = static_cast<int>(bufferSize) - sentBytes;
                    int iResult = send(Sock, modifiableBuffer.c_str(), len, 0);

                    if (iResult <= 0)
                    {
                        LastWsaError = WSAGetLastError();
                        break;
                    }

                    sentBytes += iResult;
                    modifiableBuffer = modifiableBuffer.substr(iResult, modifiableBuffer.size() - iResult);
                }

                return sentBytes;
            }
            else
            {
                std::vector<std::string> parts = UtilityLib::String::DivideByLength(buffer, INT_MAX);

                // Send each part by calling this function recursively until all of them are sent
                // Since each part is <= INT_MAX, if block will be executed instead of this else block.
                // Therefore all of the data will be sent
                for (const auto& part : parts)
                {
                    int iResult = SendAll(part);

                    if (iResult < 0)
                    {
                        LastWsaError = WSAGetLastError();
                        return iResult;
                    }
                    else if (iResult == 0)
                    {
                        return iResult;
                    }
                }
                return INT_MAX;
            }
        }
        int SocketCls::Send(const uint8_t* buffer, size_t bufLen)
        {
            std::string buf;
            if (bufLen == 0)
            {
                buf.append(reinterpret_cast<const char*>(buffer));
            }
            else
            {
                buf.append(reinterpret_cast<const char*>(buffer), bufLen);
            }
            return Send(buf);
        }
        int SocketCls::SendAll(const uint8_t* buffer, size_t bufLen)
        {
            std::string buf;

            if (bufLen == 0)
            {
                buf.append(reinterpret_cast<const char*>(buffer));
            }
            else
            {
                buf.append(reinterpret_cast<const char*>(buffer), bufLen);
            }

            return SendAll(buf);
        }
        int SocketCls::Send(const char* buffer, size_t bufLen)
        {
            std::string buf;

            if (bufLen == 0)
            {
                buf.append(buffer);
            }
            else
            {
                buf.append(buffer, bufLen);
            }

            return Send(buf);
        }
        int SocketCls::SendAll(const char* buffer, size_t bufLen)
        {
            std::string buf;

            if (bufLen == 0)
            {
                buf.append(buffer);
            }
            else
            {
                buf.append(buffer, bufLen);
            }

            return Send(buf);
        }

        int SocketCls::Recv(std::string& buffer, size_t bufLen)
        {
            char* buf = new char[bufLen];
            if (buf != nullptr)
            {
                int iResult = recv(Sock, buf, static_cast<int>(bufLen), 0);

                if (iResult < 0)
                {
                    delete[] buf;
                    LastWsaError = WSAGetLastError();
                    return SOCKET_ERROR;
                }
                if (iResult == 0)
                {
                    delete[] buf;
                    return 0;
                }

                buffer.clear();
                buffer.assign(buf, iResult);
                delete[] buf;
                return iResult;
            }
            else
            {
                LastWsaError = 0;
                return -1;
            }
        }
        int SocketCls::RecvAll(std::string& buffer)
        {
            const size_t bufSize = 512;
            char buf[bufSize] = { 0 };
            int iResult = bufSize;

            buffer.clear();

            while (iResult == bufSize)
            {
                iResult = recv(Sock, buf, bufSize, 0);

                if (iResult <= 0)
                {
                    LastWsaError = WSAGetLastError();
                    return SOCKET_ERROR;
                }

                buffer.append(buf, iResult);
            }

            int len = INT_MAX;
            if (buffer.size() < INT_MAX)
            {
                len = static_cast<int>(buffer.size());
            }
            // When buffer.size() is greater than INT_MAX

            return len;

        }
        int SocketCls::Recv(char* buffer, size_t bufLen)
        {
            if (bufLen > INT_MAX)
            {
                LastWsaError = 0;
                return SOCKET_ERROR;
            }

            int iResult = recv(Sock, buffer, static_cast<int>(bufLen), 0);

            if (iResult < 0)
            {
                LastWsaError = WSAGetLastError();
            }

            return iResult;
        }
        int SocketCls::RecvAll(char* buffer, size_t bufLen)
        {
            int iResult = 0;
            int recvBytes = 0;

            while (recvBytes < bufLen)
            {
                iResult = recv(Sock, buffer + recvBytes, static_cast<int>(bufLen) - recvBytes, 0);

                if (iResult <= 0)
                {
                    LastWsaError = WSAGetLastError();
                    return iResult;
                }
                recvBytes += iResult;
            }

            return recvBytes;
        }
        int SocketCls::Recv(uint8_t* buffer, size_t bufLen)
        {
            if (bufLen > INT_MAX)
            {
                LastWsaError = 0;
                return SOCKET_ERROR;
            }

            int iResult = recv(Sock, reinterpret_cast<char*>(buffer), static_cast<int>(bufLen), 0);

            if (iResult < 0)
            {
                LastWsaError = WSAGetLastError();
            }

            return iResult;
        }
        int SocketCls::RecvAll(uint8_t* buffer, size_t bufLen)
        {
            int iResult = 0;
            int recvBytes = 0;

            char* buf = reinterpret_cast<char*>(buffer);

            while (recvBytes < bufLen)
            {
                iResult = recv(Sock, buf + recvBytes, static_cast<int>(bufLen) - recvBytes, 0);

                if (iResult <= 0)
                {
                    LastWsaError = WSAGetLastError();
                    return iResult;
                }
                recvBytes += iResult;
            }

            return recvBytes;
        }
    };
};