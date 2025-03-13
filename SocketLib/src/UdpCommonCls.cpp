#include "UdpCommonCls.h"

namespace UtilityLib
{
    namespace Socket
    {
        UdpCommonCls::UdpCommonCls() :
            SocketCommonCls()
        {
        }
        UdpCommonCls::UdpCommonCls(UdpCommonCls&& other) noexcept :
            SocketCommonCls(std::move(other))
        {
        }
        UdpCommonCls& UdpCommonCls::operator=(UdpCommonCls&& other) noexcept
        {
            SocketCommonCls::operator=(std::move(other));
            return *this;
        }
        std::variant<WinsockError, UdpCommonCls> UdpCommonCls::Initialize(
            const addrinfo& hints,
            const std::string& ipAddress,
            const std::string& port,
            BlockingMode blockingMode)
        {
            UdpCommonCls udp;

            udp.SetHints(hints);
            if (udp.SetIpAddress(ipAddress) == false)
            {
                return WinsockError::InvalidIpAddress;
            }
            if (udp.SetPort(port) == false)
            {
                return WinsockError::InvalidPort;
            }

            WinsockError result = udp.InitializeWinsock();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = udp.GetAddressInfo();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = udp.CreateSocket();
            if (result != WinsockError::Success)
            {
                return result;
            }

            result = udp.SetBlockingMode(blockingMode);
            if (result != WinsockError::Success)
            {
                return result;
            }

            return udp;
        }

        WinsockError UdpCommonCls::RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount)
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
            sockaddr* addrPtr = reinterpret_cast<sockaddr*>(AddressInfoResults->ai_addr);
            int addrLen = static_cast<int>(sizeof(*addrPtr));

            int iResult = recvfrom(Sock, bufPtr, bufLen, 0, addrPtr, &addrLen);
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
        WinsockError UdpCommonCls::SendTo(const std::string& buffer, size_t bufferLen, size_t& sentByteCount)
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
            sockaddr* addrPtr = reinterpret_cast<sockaddr*>(AddressInfoResults->ai_addr);
            int addrLen = static_cast<int>(sizeof(*addrPtr));

            int iResult = sendto(Sock, buffer.c_str(), bufLen, 0, addrPtr, addrLen);
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