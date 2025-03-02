#ifndef SOCKETCLS_H
#define SOCKETCLS_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <string>
#include <vector>
#include <mutex>

#include "StringUtility.h"
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
            //ErrorEnum Recv(std::string&);
        };

        class SocketCls
        {
        protected:
            SOCKET Sock;

            // Client related data
            addrinfo* AddressInfoResults;

            // Common data
            addrinfo Hints;
            std::string IpAddress;
            std::string Port;

            // WSA Error Code
            int LastWsaError;

        public:
            // Construct Socket object with default values
            SocketCls();
            // Construct object with default values except addrinfo Hints
            SocketCls(const addrinfo& hints);
            // Construct object with default values except IpAddress and Port
            SocketCls(const std::string& ipAddress, const std::string& port);
            // 1. Construct object with provided values
            // 2. Initialize Winsock2
            // 3. Get Address Info according to the hints provided
            // 4. Create socket
            // 
            // Note 1: After construction, check GetLastSocketError() == 0
            // to verify object is constructed correctly
            // Otherwise, perform the 4 steps above manually
            // and check GetLastSocketError() != 0 to see the related error code
            // 
            // Note 2: Constructor will not call Connect(), call it manually when needed
            SocketCls(const addrinfo& hints, const std::string& ipAddress, const std::string& port);
            // Destroy owned resources, then object itself
            ~SocketCls();

            // InitializeWinsock()
            // 
            // Summary:
            // Initializes Winsock2 by calling WSAStartup() with 2.2 version
            // 
            // If Winsock2 successfully initialized, returns true
            // Otherwise returns false
            // Specific error code can be obtained by calling GetLastSocketError() on failure
            // 
            // Note 1: Following constructors do not call this function, explicit call is required before using any other methods
            // * Socket()
            // * Socket(const addrinfo& hints)
            // 
            // Note 2: Following constructors call this function, explicit call is not required before using any other methods
            // Socket(const addrinfo& hints, const std::string& nodeName, const std::string& serviceName)
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // bool
            bool InitializeWinsock();
            // CleanupWinsock()
            // 
            // Summary:
            // Deregister from Windows Sockets
            // If Windows2 successfully deregistered, returns true
            // Otherwise returns false
            // Specific error code can be obtained by calling GetLastSocketError() on failure
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // bool
            bool CleanupWinsock();

            // SetHints()
            // 
            // Summary:
            // Provide hints for getaddrinfo() syscall
            // 
            // Arguments:
            // const addrinfo& hints  --- In
            // 
            // Returns:
            // void
            void SetHints(const addrinfo& hints);
            // GetAddressInfoResults()
            // 
            // Summary:
            // Returns result created by GetAddressInfo()
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // addrinfo*
            addrinfo* GetAddressInfoResults();
            // GetSocket()
            // 
            // Summary:
            // Returns object's socket
            // If object's socket is invalid, INVALID_SOCKET will be returned
            // Otherwise, socket created by CreateSocket() will be returned
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // SOCKET
            SOCKET GetSocket() const;
            // GetLastSocketError()
            // 
            // Summary:
            // Returns error code of the last Winsock2 error
            // If no error is occurred yet, 0 will be returned
            // Meanings of the error codes can be checked from Microsoft Winsock2 documentation
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // int
            int GetLastSocketError() const;
            // SetLastSocketError()
            // 
            // Summary:
            // Sets last error code returned by GetLastSocketError() to a specific value
            // This can be used to clear error code after a failure is handled so that error code is not returned anymore
            // 
            // Arguments:
            // int errorCode -- In
            // 
            // Returns:
            // void
            void SetLastSocketError(int errorCode);

            bool GetAddressInfo();
            bool GetAddressInfo(const addrinfo& hints);
            // GetAddressInfo()
            // 
            // Summary:
            // Wrapper to the syscall getaddrinfo()
            // Hints must be set previously during construction or by explicitly calling SetHints()
            // On success, returns true
            // On failure, returns false. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Arguments:
            // const std::string& nodeName     --- In
            // const std::string& serviceName  --- In
            // 
            // Returns:
            // bool
            bool GetAddressInfo(const std::string& ipAddress, const std::string& port);
            // GetAddressInfo()
            // 
            // Summary:
            // Wrapper to the syscall getaddrinfo()
            // hints provided to the method will be used to call getaddrinfo()
            // Object member Hints will be updated by the provided hints
            // On success, returns true
            // On failure, returns false. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Arguments:
            // const addrinfo& hints           --- In
            // const std::string& nodeName     --- In
            // const std::string& serviceName  --- In
            // 
            // Returns:
            // bool
            bool GetAddressInfo(const addrinfo& hints, const std::string& ipAddress, const std::string& port);
            // CreateSocket()
            // 
            // Summary:
            // Wrapper to the syscall socket()
            // Creates a socket using the result of the call of GetAddressInfo()
            // On success, returns true
            // On failure, returns false. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // bool
            bool CreateSocket();
            // CloseSocket()
            // 
            // Summary:
            // Closes the object's existing socket
            // GetSocket() will return INVALID_SOCKET after successful call of this method
            // On success, returns true
            // On failure, returns false. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // bool
            bool CloseSocket();
            // Connect()
            // 
            // Summary:
            // Connects socket created by CreateSocket() to a remote server
            // On success, returns true
            // On failure, return false. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Arguments:
            // -
            // 
            // Returns:
            // bool
            bool Connect();
            // Bind()
            // 
            // Summary
            bool Bind();


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
            bool SetBlockingMode(BlockingMode mode);

            // Send()
            // 
            // Summary:
            // Wrapper to the syscall send()
            // On success, returns the amount of bytes sent.
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Note 1:
            // No promise on the successful send of all bytes.
            // It is on the caller's responsibility to check the return value
            // and if not all bytes are sent, rearrange the buffer and try again
            // To send all bytes, consider using SendAll()
            // 
            // Note 2:
            // If size of buffer is higher than INT_MAX, then no data will be send.
            // Return value will be SOCKET_ERROR
            // GetLastSocketError() will return 0
            // 
            // Arguments:
            // const std::string& buffer  --- In
            // 
            // Returns:
            // int
            int Send(const std::string& buffer);
            // SendAll()
            // 
            // Summary:
            // Sends all bytes in buffer
            // On success, returns the amount of bytes sent
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Note 1:
            // If buffer is longer than INT_MAX, on successful send, INT_MAX will be returned
            // 
            // Note 2:
            // If only part of the bytes sent, then a failure occurred. Return value will be -1
            // Independent of the number of bytes sent
            // 
            // Arguments:
            // const std::string& buffer  --- In
            // 
            // Returns:
            // int
            int SendAll(const std::string& buffer);
            // Send()
            // 
            // Summary:
            // Wrapper to the syscall send()
            // On success, returns the amount of bytes sent.
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Note 1:
            // No promise on the successful send of all bytes.
            // It is on the caller's responsibility to check the return value
            // and if not all bytes are sent, rearrange the buffer and try again
            // To send all bytes, consider using SendAll()
            // 
            // Note 2:
            // If size of buffer is higher than INT_MAX, then no data will be send.
            // Return value will be SOCKET_ERROR
            // GetLastSocketError() will return 0
            // 
            // Note 3:
            // If bufLen is not provided or provided as 0, bytes will be send until \0 is reached
            // Otherwise bufLen amount of bytes will be send
            // 
            // Arguments:
            // const uint8_t* buffer  --- In
            // size_t bufLen = 0      --- In
            // 
            // Returns:
            // int
            int Send(const uint8_t* buffer, size_t bufLen = 0);
            // SendAll()
            // 
            // Summary:
            // Sends all bytes in buffer
            // On success, returns the amount of bytes sent
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Note 1:
            // If buffer is longer than INT_MAX, on successful send, INT_MAX will be returned
            // 
            // Note 2:
            // If only part of the bytes sent, then a failure occurred. Return value will be sent bytes
            // It is on the caller's responsibility to fix the error
            // and try sending again rest of the bytes or resend all of the bytes again
            // 
            // Note 3:
            // If bufLen is not provided or provided as 0, bytes will be send until \0 is reached
            // Otherwise bufLen amount of bytes will be send
            // 
            // Arguments:
            // const uint8_t* buffer  --- In
            // size_t bufLen = 0      --- In
            // 
            // Returns:
            // int
            int SendAll(const uint8_t* buffer, size_t bufLen = 0);
            // Send()
            // 
            // Summary:
            // Wrapper to the syscall send()
            // On success, returns the amount of bytes sent.
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Note 1:
            // No promise on the successful send of all bytes.
            // It is on the caller's responsibility to check the return value
            // and if not all bytes are sent, rearrange the buffer and try again
            // To send all bytes, consider using SendAll()
            // 
            // Note 2:
            // If size of buffer is higher than INT_MAX, then no data will be send.
            // Return value will be SOCKET_ERROR
            // GetLastSocketError() will return 0
            // 
            // Note 3:
            // If bufLen is not provided or provided as 0, bytes will be send until \0 is reached
            // Otherwise bufLen amount of bytes will be send
            // 
            // Arguments:
            // const char* buffer  --- In
            // size_t bufLen = 0   --- In
            // 
            // Returns:
            // int
            int Send(const char* buffer, size_t bufLen = 0);
            // SendAll()
            // 
            // Summary:
            // Sends all bytes in buffer
            // On success, returns the amount of bytes sent
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Note 1:
            // If buffer is longer than INT_MAX, on successful send, INT_MAX will be returned
            // 
            // Note 2:
            // If only part of the bytes sent, then a failure occurred. Return value will be sent bytes
            // It is on the caller's responsibility to fix the error
            // and try sending again rest of the bytes or resend all of the bytes again
            // 
            // Note 3:
            // If bufLen is not provided or provided as 0, bytes will be send until \0 is reached
            // Otherwise bufLen amount of bytes will be send
            // 
            // Arguments:
            // const char* buffer  --- In
            // size_t bufLen = 0   --- In
            // 
            // Returns:
            // int
            int SendAll(const char* buffer, size_t bufLen = 0);

            // Recv()
            // 
            // Summary:
            // Wrapper to the syscall recv()
            // On success, returns the amount of bytes received, up to specified buffer length.
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // On connection to server is closed gracefully, returns 0
            // 
            // Note 1: If there are more bytes than "bufLen" in socket, 
            // then it is up to the caller to call this function again to receive remaining bytes
            // 
            // Note 2: No need to reserve space for std::string buffer, 
            // it will be handled in function based on provided size_t bufLen
            // 
            // Arguments:
            // std::string& buffer  --- Out
            // size_t bufLen        --- In
            // 
            // Returns:
            // int
            int Recv(std::string& buffer, size_t bufLen);
            // RecvAll()
            // 
            // Summary:
            // Receive all existing data on the socket
            // On success, returns the amount of bytes received
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Note 1:
            // If connection to the server is gracefully closed, return value will be 0
            // Connection should be reestablished before receiving data
            // 
            // Arguments:
            // std::string& buffer  --- Out
            // 
            // Returns:
            // int
            int RecvAll(std::string& buffer);
            // Recv()
            // 
            // Summary:
            // Wrapper to the syscall recv()
            // On success, returns the amount of bytes received, up to specified buffer length.
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // On connection to server is closed gracefully, returns 0
            // 
            // Note 1: If there are more bytes than "bufLen" in socket, 
            // then it is up to the caller to call this function again to receive remaining bytes
            // 
            // Note 2: Do not pass bufSize greater than INT_MAX, 
            // otherwise return value will be SOCKET_ERROR and GetLastSocketError() will be 0
            // 
            // Arguments:
            // char* buffer   --- Out
            // size_t bufLen  --- In
            // 
            // Returns:
            // int
            int Recv(char* buffer, size_t bufLen);
            // RecvAll()
            // 
            // Summary:
            // Receive all existing data on the socket
            // On success, returns the amount of bytes received
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Note 1:
            // If connection to the server is gracefully closed, return value will be 0
            // Connection should be reestablished before receiving data
            // 
            // Arguments:
            // char* buffer   --- Out
            // size_t bufLen  --- In
            // 
            // Returns:
            // int
            int RecvAll(char* buffer, size_t bufLen);
            // Recv()
            // 
            // Summary:
            // Wrapper to the syscall recv()
            // On success, returns the amount of bytes received, up to specified buffer length.
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // On connection to server is closed gracefully, returns 0
            // 
            // Note 1: If there are more bytes than "bufLen" in socket, 
            // then it is up to the caller to call this function again to receive remaining bytes
            // 
            // Note 2: Do not pass bufSize greater than INT_MAX, 
            // otherwise return value will be SOCKET_ERROR and GetLastSocketError() will be 0
            // 
            // Arguments:
            // uint8_t* buffer   --- Out
            // size_t bufLen     --- In
            // 
            // Returns:
            // int
            int Recv(uint8_t* buffer, size_t bufLen);
            // RecvAll()
            // 
            // Summary:
            // Receive all existing data on the socket
            // On success, returns the amount of bytes received
            // On failure, returns SOCKET_ERROR. Specific error code can be obtained
            // by calling GetLastSocketError() on failure
            // 
            // Note 1:
            // If connection to the server is gracefully closed, return value will be 0
            // Connection should be reestablished before receiving data
            // 
            // Arguments:
            // uint8_t* buffer   --- Out
            // size_t bufLen     --- In
            // 
            // Returns:
            // int
            int RecvAll(uint8_t* buffer, size_t bufLen);
        };
    };
};

#endif