#ifndef TCPSESSIONCLS_H
#define TCPSESSIONCLS_H

#include "SocketTypePkg.h"

namespace UtilityLib
{
    namespace Socket
    {
        class TcpSessionCls
        {
        private:
            SOCKET Sock;
            int LastWinsockError;
            std::string IpAddress;
            std::string Port;
            char Buffer[2048];

        public:
            // Constructor to be used by TcpServerCls::Accept
            // Do not manually create an object through this
            TcpSessionCls(SOCKET clientHandlerSock, const std::string& ipAddress, const std::string& port);

            // GetLastWinsockError()
            // 
            // Summary:
            // Returns last Winsock2 internal error code
            // 
            // Arguments:
            // 
            // Returns:
            // int
            int GetLastWinsockError();

            // SetBlockingMode()
            // 
            // Summary:
            // Change blocking mode of the socket
            // 
            // Arguments:
            // BlockingMode mode  --- In
            // 
            // Returns:
            // WinsockError
            WinsockError SetBlockingMode(BlockingMode mode);

            // Recv()
            // 
            // Summary:
            // Receive data from the connected TCP Client
            // Check recvByteCount to see how many bytes are received, call this again if all data is not received
            // 
            // Arguments:
            // std::string& buffer    --- Out (Just pass a default constructed buffer, necessary allocation will be handled inside)
            // size_t bufferLen       --- In (Try not to exceed 2048 bytes, there will be performance penalties if you do)
            // size_t& recvByteCount  --- Out
            // 
            // Returns:
            // WinsockError
            // 
            // On failure:
            // * WinsockError::BufferTooLong         is returned when bufferLen > INT_MAX
            // * WinsockError::BufferLengthIsZero    is returned when bufferLen == 0
            // * WinsockError::OutOfMemory           is returned when buffer allocation fails
            // * WinsockError::NotInitialized        is returned when Winsock2 socket is not created due to an previous error
            //                                       (I don't expect this to occur ever since Initialize() does not return an object if socket creation fails
            //                                        yet nothing wrong to make sure)
            // * WinsockError::CheckLastWinsockError is returned when an internal Winsock2 error occurs.
            //                                       call GetLastWinsockError() to receive error code, then check Microsoft's documentation
            // 
            // On success:
            // * WinsockError::Success               is returned and recvByteCount is set to received byte count
            WinsockError Recv(std::string& buffer, size_t bufferLen, size_t& recvByteCount);

            // Send()
            // 
            // Summary:
            // Send data from specified IP Address and Port during initialization
            // Check sentByteCount to see how many bytes are sent, call this again if all data is not sent
            // 
            // Arguments:
            // const std::string& buffer  --- In
            // size_t bufferLen           --- In
            // size_t& sentByteCount      --- Out
            // 
            // Returns:
            // WinsockError
            // 
            // On failure:
            // * WinsockError::BufferTooLong         is returned when bufferLen > INT_MAX
            // * WinsockError::BufferLengthIsZero    is returned when bufferLen == 0
            // * WinsockError::NotInitialized        is returned when Winsock2 socket is not created due to an previous error
            //                                       (I don't expect this to occur ever since Initialize() does not return an object if socket creation fails
            //                                        yet nothing wrong to make sure)
            // * WinsockError::CheckLastWinsockError is returned when an internal Winsock2 error occurs.
            //                                       call GetLastWinsockError() to receive error code, then check Microsoft's documentation
            // 
            // On success:
            // * WinsockError::Success               is returned and sentByteCount is set to sent byte count
            WinsockError Send(const std::string& buffer, size_t bufferLen, size_t& sentByteCount);

            // Move constructor
            TcpSessionCls(TcpSessionCls&& other) noexcept;
            // Move assigment operator
            TcpSessionCls& operator=(TcpSessionCls&& other) noexcept;
            // Default constructor is deleted
            TcpSessionCls() = delete;
            // Copy constructor is deleted
            TcpSessionCls(const TcpSessionCls&) = delete;
            // Copy assignment operator is deleted
            TcpSessionCls& operator=(const TcpSessionCls&) = delete;
            // Destructor: freeaddrinfo and closesocket are called appropriately, do not worry..
            ~TcpSessionCls();
        };
    }
}

#endif