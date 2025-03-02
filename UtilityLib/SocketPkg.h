#ifndef SOCKETPKG_H
#define SOCKETPKG_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <string>
#include <vector>
#include <mutex>

#include "StringPkg.h"
#include "ErrorPkg.h"

using namespace UtilityLib::Error;

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

        class SocketCommonCls : public WSAErrorCls
        {
        protected:
            SOCKET Sock;
            addrinfo Hints;
            addrinfo* AddressInfoResults;
            std::string IpAddress;
            std::string Port;

        public:
            SocketCommonCls();
            SocketCommonCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port);
            ~SocketCommonCls();
            // InitializeWinsock()
            // 
            // Summary:
            // Initializes Winsock2 by calling WSAStartup() with 2.2 version
            // A successfull call to this function must be made before using any other functions
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // On success ErrorEnum::Success
            // On failure ErrorEnum::WinsockError
            // 
            // Specific error code can be obtained by 
            // calling GetLastWsaError() or GetLastWsaErrorAsync() on failure
            ErrorEnum InitializeWinsock();
            // CleanupWinsock()
            // 
            // Summary:
            // Cleanup Winsock2
            // For every sucessfull call to the InitializeWinsock()
            // There must be equal call to the CleanupWinsock()
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // On success ErrorEnum::Success
            // On failure ErrorEnum::WinsockError
            // 
            // Specific error code can be obtained by 
            // calling GetLastWsaError() or GetLastWsaErrorAsync() on failure
            ErrorEnum CleanupWinsock();
            // SetHints()
            // 
            // Summary:
            // Provide hints
            // 
            // Arguments:
            // const addrinfo& hints  --- In
            // 
            // Returns:
            // void
            void SetHints(const addrinfo& hints);
            // SetIpAddress()
            // 
            // Summary:
            // Provide IP Adress
            // 
            // Arguments:
            // const addrinfo& hints  --- In
            // 
            // Returns:
            // void
            void SetIpAddress(const std::string& ipAddress);
            // SetPort()
            // 
            // Summary:
            // Provide Port
            // 
            // Arguments:
            // const addrinfo& hints  --- In
            // 
            // Returns:
            // void
            void SetPort(const std::string& port);
            ErrorEnum GetAddressInfo();
            ErrorEnum GetAddressInfo(const addrinfo& hints, const std::string& ipAddress, const std::string& port);
            // CreateSocket()
            // 
            // Summary:
            // Wrapper to the socket()
            // Creates socket
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // On success ErrorEnum::Success
            // On failure ErrorEnum::WinsockError
            // 
            // Specific error code can be obtained by 
            // calling GetLastWsaError() or GetLastWsaErrorAsync() on failure
            ErrorEnum CreateSocket();
            // CloseSocket()
            // 
            // Summary:
            // Closes the object's existing socket
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // On success ErrorEnum::Success
            // On failure ErrorEnum::WinsockError
            // 
            // Specific error code can be obtained by 
            // calling GetLastWsaError() or GetLastWsaErrorAsync() on failure
            ErrorEnum CloseSocket();
            // Recv()
            // 
            // Summary:
            // Receive data
            // To see how much bytes are received, check buffer.size() upon return
            // 
            // Arguments:
            // std::string& buffer  --- Out
            // size_t bufferLength  --- In
            // 
            // Returns:
            // On success ErrorEnum::Success
            // On failure ErrorEnum::WinsockError or,
            //            ErrorEnum::WinsockConnClosed or, (if recv() returns 0,  which means server closed connection)
            //            ErrorEnum::InsufficientMemory or, (if memory allocation fails)
            //            ErrorEnum::InvalidArgument (if bufferLength > INT_MAX)
            // 
            // Specific error code can be obtained by
            // calling GetLastWsaError() or GetLastWsaErrorAsync() on failure
            ErrorEnum Recv(std::string& buffer, size_t bufferLength);
            // Send()
            // 
            // Summary:
            // Send data
            // 
            // Arguments:
            // const std::string& buffer  --- In
            // size_t& bytesSent          --- Out
            // 
            // Returns:
            // On success ErrorEnum::Success
            // On failure ErrorEnum::WinsockError or,
            //            ErrorEnum::OutOfRange or, (if buffer.size() > INT_MAX)
            //            ErrorEnum::WinsockConnClosed (if recv() returns 0,  which means server closed connection)
            // 
            // Specific error code can be obtained by
            // calling GetLastWsaError() or GetLastWsaErrorAsync() on failure
            ErrorEnum Send(const std::string& buffer, size_t& bufferLength);
            // SetBlockingMode()
            // 
            // Summary:
            // Sets blocking mode of the socket
            // On success, returns true
            // On failure, returns false. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Arguments:
            // BlockingModeEnum mode  --- In
            // 
            // Returns:
            // bool
            ErrorEnum SetBlockingMode(BlockingMode mode);
        };

        class SocketServerCls : public SocketCommonCls
        {
        public:
            std::vector<SOCKET> clientSockets;

            SocketServerCls();
            SocketServerCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port);
            
            ErrorEnum Bind();
            ErrorEnum Listen();
            ErrorEnum Accept();
        };

        class SocketClientCls : public SocketCommonCls
        {
        private:
            addrinfo* AddressInfoResults;
        public:
            SocketClientCls();
            SocketClientCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port);

            ErrorEnum Connect();
        };

        class ThreadedServerCls
        {

        };
    };
};

#endif