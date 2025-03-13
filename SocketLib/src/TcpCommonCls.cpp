#include "TcpCommonCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        TcpCommonCls::TcpCommonCls() :
            SocketCommonCls()
        {
        }
        TcpCommonCls::TcpCommonCls(TcpCommonCls&& other) noexcept :
            SocketCommonCls(std::move(other))
        {
        }
        TcpCommonCls& TcpCommonCls::operator=(TcpCommonCls&& other) noexcept
        {
            SocketCommonCls::operator=(std::move(other));
            return *this;
        }

        std::variant<WinsockError, TcpCommonCls> TcpCommonCls::Initialize(
            const addrinfo& hints,
            const std::string& ipAddress,
            const std::string& port,
            BlockingMode blockingMode)
        {
            TcpCommonCls tcp;
            tcp.SetHints(hints);
            if (tcp.SetIpAddress(ipAddress) == false)
            {
                return WinsockError::InvalidIpAddress;
            }
            if (tcp.SetPort(port) == false)
            {
                return WinsockError::InvalidPort;
            }
            WinsockError result = tcp.InitializeWinsock();
            if (result != WinsockError::Success)
            {
                return result;
            }
            result = tcp.GetAddressInfo();
            if (result != WinsockError::Success)
            {
                return result;
            }
            result = tcp.CreateSocket();
            if (result != WinsockError::Success)
            {
                return result;
            }
            result = tcp.SetBlockingMode(blockingMode);
            if (result != WinsockError::Success)
            {
                return result;
            }
            return tcp;
        }

        WinsockError TcpCommonCls::Send(const std::string& buffer, size_t bufferLen, size_t& sentByteCount)
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
            int iResult = send(Sock, bufPtr, bufLen, 0);
            if (iResult == SOCKET_ERROR)
            {
                LastWinsockError = WSAGetLastError();
                return WinsockError::CheckLastWinsockError;
            }
            sentByteCount = static_cast<size_t>(iResult);
            return WinsockError::Success;
        }
        WinsockError TcpCommonCls::Recv(std::string& buffer, size_t bufferLen, size_t& recvByteCount)
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
            int iResult = recv(Sock, bufPtr, bufLen, 0);
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
    }
}