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
            ~SocketCommonCls();
            int GetLastWinsockError() const;

        protected:
            SOCKET GetSocket() const;
            addrinfo GetHints() const;
            addrinfo* GetAddressInfoResults() const;
            std::string GetIpAddress() const;
            std::string GetPort() const;
            
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

        private:
            WinsockError Bind();
        };

        // Common methods for TCP Server and Client
        class TcpCommonCls : public SocketCommonCls
        {
        protected:
            TcpCommonCls();

        public:
            static std::variant<WinsockError, TcpCommonCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);

            TcpCommonCls(TcpCommonCls&& other) noexcept;
            TcpCommonCls& operator=(TcpCommonCls&& other) noexcept;

            WinsockError Send(const std::string& buffer, size_t bufferLen, size_t& sentByteCount);
            WinsockError Recv(std::string& buffer, size_t bufferLen, size_t& recvByteCount);
        };

        class TcpClientCls : public TcpCommonCls
        {
        private:
            TcpClientCls();
            TcpClientCls(TcpCommonCls&& other) noexcept;
            TcpClientCls& operator=(TcpCommonCls&& other) noexcept;

        public:
            static std::variant<WinsockError, TcpClientCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);
            TcpClientCls(TcpClientCls&& other) noexcept;
            TcpClientCls& operator=(TcpClientCls&& other) noexcept;

        private:
            WinsockError Connect();
        };

        class TcpServerCls : public TcpCommonCls
        {
        private:
            TcpServerCls();
            TcpServerCls(TcpCommonCls&& other) noexcept;
            TcpServerCls& operator=(TcpCommonCls&& other) noexcept;

        public:
            static std::variant<WinsockError, TcpServerCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);
            TcpServerCls(TcpServerCls&& other) noexcept;
            TcpServerCls& operator=(TcpServerCls&& other) noexcept;

        private:
            WinsockError Bind();

        public:
            WinsockError Listen(int backlog);
            std::variant<WinsockError, TcpCommonCls> Accept();
        };

        class TcpClientHandlerCls : public TcpCommonCls
        {
        public:
            TcpClientHandlerCls(SOCKET clientHandlerSock);

        private:
            TcpClientHandlerCls();
        };
    };
};

#endif