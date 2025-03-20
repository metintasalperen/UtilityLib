#ifndef UDPSERVERCLS_H
#define UDPSERVERCLS_H

#include "SocketTypePkg.h"
#include "StringPkg.h"

#include <variant>

namespace UtilityLib
{
    namespace Socket
    {
        class UdpServerCls
        {
            // Will not comment private attributes and methods because they are for internal use
            // Just go use public methods, you don't need to know these
            // Thx XOXO
        private:
            SOCKET Sock;
            std::string Port;
            int LastWinsockError;
            char Buffer[2048];

            UdpServerCls();

            bool SetPort(const std::string& port);

            WinsockError CreateSocket();
            WinsockError CloseSocket();
            WinsockError SetBlockingMode(BlockingMode mode);
            WinsockError Bind();

        public:
            // Initialize
            // 
            // Summary:
            // Initialize as socket to use as a UDP Server
            // 
            // Arguments:
            // const std::string& port       --- In
            // BlockingMode mode             --- In (default BlockingMode::Blocking)
            // 
            // Returns:
            // std::variant<WinsockError, UdpServerCls>
            // 
            // If initialization is successful, a UdpServerCls object will be returned
            // 
            // If initialization is not successful,
            // WinsockError::InvalidPort           is returned when provided Port is not valid
            // WinsockError::CheckLastWinsockError is returned when an internal Winsock2 error occurs.
            //                                     call GetLastWinsockError() to receive error code, then check Microsoft's documentation
            // 
            // Note: Once initialized, Port cannot be changed
            // Create a new object by calling this method if you need to change it...
            static std::variant<WinsockError, UdpServerCls> Initialize(
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking);

            // RecvFrom()
            // 
            // Summary:
            // Receive data from an UDP Client
            // Check recvByteCount to see how many bytes are received, call this again if all data is not received
            // 
            // Arguments:
            // std::string& buffer      --- Out (Just pass a default constructed buffer, necessary allocation will be handled inside)
            // size_t bufferLen         --- In (Try not to exceed 2048 bytes, there will be performance penalties if you do)
            // size_t& recvByteCount    --- Out
            // std::string& fromIpAddr  --- Out (Ip Address of UDP Client)
            // std::string& fromPort    --- Out (Port of UDP Client)
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
            WinsockError RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount, std::string& fromIpAddr, std::string& fromPort);

            // SendTo()
            // 
            // Summary:
            // Send data to specified IP Address and Port
            // Check sentByteCount to see how many bytes are sent, call this again if all data is not sent
            // 
            // Arguments:
            // const std::string& buffer    --- In
            // size_t bufferLen             --- In
            // size_t& sentByteCount        --- Out
            // const std::string& toIpAddr  --- In (Ip address of UDP Client that packet will be send to)
            // const std::string& toPort    --- In (Port of UDP Client that packet will be send to)
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
            WinsockError SendTo(const std::string& buffer, size_t bufferLen, size_t& sentByteCount, const std::string& toIpAddr, const std::string& toPort);

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

            // Move constructor
            UdpServerCls(UdpServerCls&& other) noexcept;
            // Move assignment operator
            UdpServerCls& operator=(UdpServerCls&& other) noexcept;
            // Copy constructor is deleted
            UdpServerCls(const UdpServerCls&) = delete;
            // Copy assignment operator is deleted
            UdpServerCls& operator=(const UdpServerCls&) = delete;
            // Destructor: closesocket is called appropriately, do not worry..
            ~UdpServerCls();
        };
    }
}

#endif