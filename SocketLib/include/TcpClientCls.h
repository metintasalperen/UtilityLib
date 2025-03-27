#ifndef TCPCLIENTCLS_H
#define TCPCLIENTCLS_H

#include "StringPkg.h"
#include "SocketTypePkg.h"

#include <variant>

namespace UtilityLib
{
    namespace Socket
    {
        class TcpClientCls
        {
        private:
            SOCKET Sock;
            addrinfo Hints;
            addrinfo* AddressInfoResults;
            std::string IpAddress;
            std::string Port;
            int LastWinsockError;
            char Buffer[2048];

            TcpClientCls();

            bool SetIpAddress(const std::string& ipAddress);
            bool SetPort(const std::string& port);

            WinsockError GetAddressInfo();
            WinsockError CreateSocket();
            WinsockError CloseSocket();
            WinsockError Connect();
        public:
            // Initialize
            // 
            // Summary:
            // Initialize as socket to use as a TCP Client
            // 
            // Arguments:
            // const addrinfo& hints         --- In
            // const std::string& ipAddress  --- In
            // const std::string& port       --- In
            // BlockingMode mode             --- In (default BlockingMode::Blocking)
            // 
            // Returns:
            // std::variant<WinsockError, TcpClientCls>
            // 
            // If initialization is successful, a TcpClientCls object will be returned
            // 
            // If initialization is not successful,
            // WinsockError::InvalidIpAddress      is returned when provided IP Address is not valid
            // WinsockError::InvalidPort           is returned when provided Port is not valid
            // WinsockError::CheckLastWinsockError is returned when an internal Winsock2 error occurs.
            //                                     call GetLastWinsockError() to receive error code, then check Microsoft's documentation
            // 
            // Note: Once initialized, IP Address and Port cannot be changed
            // Create a new object by calling this method if you need to change them...
            static std::variant<WinsockError, TcpClientCls> Initialize(
                const addrinfo& hints,
                const std::string& ipAddress,
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);

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
            // Receive data from specified IP Address and Port during initialization
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
            // Send data to specified IP Address and Port during initialization
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
            TcpClientCls(TcpClientCls&& other) noexcept;
            // Move assignment operator
            TcpClientCls& operator=(TcpClientCls&& other) noexcept;
            // Copy constructor is deleted
            TcpClientCls(const TcpClientCls&) = delete;
            // Copy assignment operator is deleted
            TcpClientCls& operator=(const TcpClientCls&) = delete;
            // Destructor: freeaddrinfo and closesocket are called appropriately, do not worry..
            ~TcpClientCls();
        };
    }
}

#endif