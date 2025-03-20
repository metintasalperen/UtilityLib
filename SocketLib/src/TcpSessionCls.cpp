#include "TcpSessionCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        TcpSessionCls::TcpSessionCls(SOCKET clientHandlerSock, const std::string& ipAddress, const std::string& port) :
            Sock(clientHandlerSock),
            IpAddress(ipAddress),
            Port(port),
            LastWinsockError(0)
        {
            memset(Buffer, 0, sizeof(Buffer));
        }
        TcpSessionCls::TcpSessionCls(TcpSessionCls&& other) noexcept :
            Sock(other.Sock),
            IpAddress(std::move(other.IpAddress)),
            Port(std::move(other.Port)),
            LastWinsockError(other.LastWinsockError)
        {
            memcpy(Buffer, other.Buffer, sizeof(Buffer));

            other.Sock = INVALID_SOCKET;
        }
        TcpSessionCls& TcpSessionCls::operator=(TcpSessionCls&& other) noexcept
        {
            if (this != &other)
            {
                Sock = other.Sock;
                IpAddress = std::move(other.IpAddress);
                Port = std::move(other.Port);
                LastWinsockError = other.LastWinsockError;
                memcpy(Buffer, other.Buffer, sizeof(Buffer));

                other.Sock = INVALID_SOCKET;
            }
            return *this;
        }
        TcpSessionCls::~TcpSessionCls()
        {
            if (Sock != INVALID_SOCKET)
            {
                closesocket(Sock);
            }
        }

        int TcpSessionCls::GetLastWinsockError()
        {
            return LastWinsockError;
        }
        WinsockError TcpSessionCls::SetBlockingMode(BlockingMode mode)
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

        WinsockError TcpSessionCls::Recv(std::string& buffer, size_t bufferLen, size_t& recvByteCount)
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
        WinsockError TcpSessionCls::Send(const std::string& buffer, size_t bufferLen, size_t& sentByteCount)
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