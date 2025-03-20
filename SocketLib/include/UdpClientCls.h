#ifndef UDPCLIENTCLS_H
#define UDPCLIENTCLS_H

#include "SocketTypePkg.h"
#include "StringPkg.h"

#include <variant>

namespace UtilityLib
{
    namespace Socket
    {
        class UdpClientCls
        {
            // Will not comment private attributes and methods because they are for internal use
            // Just go use public methods, you don't need to know these
            // Thx XOXO
        private:
            SOCKET Sock;
            std::string IpAddress;
            std::string Port;
            int LastWinsockError;
            char Buffer[2048];

            UdpClientCls();

            WinsockError CreateSocket();
            WinsockError CloseSocket();

        public:
            // Initialize
            // 
            // Summary:
            // Initialize as socket to use as a UDP Client
            // 
            // Arguments:
            // const std::string& ipAddress  --- In
            // const std::string& port       --- In
            // BlockingMode mode             --- In (default BlockingMode::Blocking)
            // 
            // Returns:
            // std::variant<WinsockError, UdpClientCls>
            // 
            // If initialization is successful, an UdpClientCls object will be returned
            // 
            // If initialization is not successful,
            // WinsockError::InvalidIpAddress      is returned when provided IP Address is not valid
            // WinsockError::InvalidPort           is returned when provided Port is not valid
            // WinsockError::CheckLastWinsockError is returned when an internal Winsock2 error occurs.
            //                                     call GetLastWinsockError() to receive error code, then check Microsoft's documentation
            // 
            // Note: Once initialized, IP Address and Port can be changed via SetIpAddress() and SetPort()
            // Check return value to make sure change is successful
            static std::variant<WinsockError, UdpClientCls> Initialize(
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

            // SetIpAddress()
            // 
            // Summary:
            // Change UDP Server IP Address
            // 
            // Arguments:
            // const std::string& ipAddress  --- In
            // 
            // Returns:
            // bool
            bool SetIpAddress(const std::string& ipAddress);

            // SetPort()
            // 
            // Summary:
            // Change UDP Server Port
            // 
            // Arguments:
            // const std::string& port  --- In
            // 
            // Returns:
            // bool
            bool SetPort(const std::string& port);

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

            // RecvFrom()
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
            WinsockError RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount);
            WinsockError RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount, std::string& fromIpAddr, std::string& fromPort);

            // SendTo()
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
            WinsockError SendTo(const std::string& buffer, size_t bufferLen, size_t& sentByteCount);

            // Move constructor
            UdpClientCls(UdpClientCls&& other) noexcept;
            // Move assignment operator
            UdpClientCls& operator=(UdpClientCls&& other) noexcept;
            // Copy constructor is deleted
            UdpClientCls(const UdpClientCls&) = delete;
            // Copy assignment operator is deleted
            UdpClientCls& operator=(const UdpClientCls&) = delete;
            // Destructor: closesocket is called appropriately, do not worry..
            ~UdpClientCls();
        };
    }
}

#endif