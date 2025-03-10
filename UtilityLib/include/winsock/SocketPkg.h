#ifndef SOCKETCLS_H
#define SOCKETCLS_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <string>
#include <variant>
#include <mutex>

#include "StringPkg.h"

#pragma comment(lib, "Ws2_32.lib")

namespace UtilityLib
{
    namespace Network
    {
        enum class BlockingMode
        {
            Blocking = 0,
            NonBlocking = 1
        };
        enum class WinsockError
        {
            Success = 0,
            InvalidIpAddress,
            InvalidPort,
            BufferTooLong,
            NotInitialized,
            OutOfMemory,
            CheckLastWinsockError,
        };

        class WinsockInitializerCls
        {
        private:
            static std::mutex Mutex;
            static uint32_t InstanceCount;

        public:
            static int GetInstanceCount()
            {
                std::lock_guard<std::mutex> lock(Mutex);
                return InstanceCount;
            }
            static void IncrementInstanceCount()
            {
                std::lock_guard<std::mutex> lock(Mutex);
                InstanceCount++;
            }
            static void DecrementInstanceCount()
            {
                std::lock_guard<std::mutex> lock(Mutex);
                if (InstanceCount > 0)
                    InstanceCount--;
            }
        };

        // Common methods for all classes
        class SocketCommonCls
        {
        protected:
            SOCKET Sock;
            addrinfo Hints;
            addrinfo* AddressInfoResults;
            std::string IpAddress;
            std::string Port;
            int LastWinsockError;

            SocketCommonCls();
            void SetHints(const addrinfo& hints);
            bool SetIpAddress(const std::string& ipAddress);
            bool SetPort(const std::string& port);

            SocketCommonCls(SocketCommonCls&& other) noexcept;
            SocketCommonCls& operator=(SocketCommonCls&& other) noexcept;

        public:
            SOCKET GetSocket() const;
            addrinfo GetHints() const;
            addrinfo* GetAddressInfoResults() const;
            std::string GetIpAddress() const;
            std::string GetPort() const;
            int GetLastWinsockError() const;
            
            ~SocketCommonCls();
            WinsockError InitializeWinsock();
            WinsockError CleanupWinsock();
            WinsockError CloseSocket();
            WinsockError SetBlockingMode(BlockingMode mode);
            WinsockError GetAddressInfo();
            WinsockError CreateSocket(); 
        };

        // Common methods for UDP Server and Client
        class UdpCommonCls : public SocketCommonCls
        {
        protected:
            UdpCommonCls();

        public:
            static std::variant<WinsockError, UdpCommonCls> Initialize(
                const addrinfo& hints, 
                const std::string& ipAddress, 
                const std::string& port, 
                BlockingMode mode = BlockingMode::Blocking);

            UdpCommonCls(UdpCommonCls&& other) noexcept;
            UdpCommonCls& operator=(UdpCommonCls&& other) noexcept;

            WinsockError RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount);
            WinsockError SendTo(const std::string& buffer, size_t bufferLen, size_t& sentByteCount);
        };

        // UDP Client does not need anything else, UdpCommonCls can be used as UdpClientCls directly
        typedef UdpCommonCls UdpClientCls;

        // UDP Server
        class UdpServerCls : public UdpCommonCls
        {
        private:
            UdpServerCls();

            UdpServerCls(UdpCommonCls&& other) noexcept;
            UdpServerCls& operator=(UdpCommonCls&& other) noexcept;

        public:
            static std::variant<WinsockError, UdpServerCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);

            UdpServerCls(UdpServerCls&& other) noexcept;
            UdpServerCls& operator=(UdpServerCls&& other) noexcept;

            WinsockError Bind();
        };
    };
};

#endif