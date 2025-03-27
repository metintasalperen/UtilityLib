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
            WinsockError Bind(const std::string& ipAddress = "");

        public:
            // Initialize()
            // 
            // Summary:
            // Initialize a socket to use as a UDP Server
            // 
            // Arguments:
            // const std::string& port       --- In
            // BlockingMode blockingMode     --- In (default BlockingMode::Blocking)
            // const std::string& ipAddress  --- In (default "", do not pass an ipAddress if you prefer to recvfrom any address, pass and ipAddress if you prefer to recvfrom only that one)
            // 
            // Returns:
            // std::variant<WinsockError, UdpServerCls>
            // 
            // If initialization is successful, a new UdpServerCls object will be returned
            // If initialization is not successful,
            // WinsockError::InvalidPort           is returned when provided Port is not a valid Port
            // WinsockError::CheckLastWinsockError is returned when when an internal Winsock2 error occurs.
            //                                     call GetLastWinsockError() to received Error Code, then check Winsock2 documentation
            // 
            // Note: Port cannot be changed after initialization
            // Create a new object through this method if you need a new socket on a different port
            static std::variant<WinsockError, UdpServerCls> Initialize(
                const std::string& port,
                BlockingMode blockingMode = BlockingMode::Blocking,
                const std::string& ipAddress = "");

            // RecvFrom()
            // 
            // Summary:
            // Receive data from an UDP Client
            // Check recvByteCount to see how many bytes are received, call this again if all data is not received
            // 
            // Arguments:
            // std::string& buffer      --- Out (Do not allocate bufferLen size for buffer, it will be handled inside. Just pass a default constructed std::string)
            // size_t bufferLen         --- In  (Try not to pass 2048 bytes, there will be performance penalties if you do)
            // size_t& recvByteCount    --- Out
            // std::string& fromIpAddr  --- Out (Set to IP Address of UDP Client)
            // std::string& fromPort    --- Out (Set to Port of UDP Client)
            // 
            // Returns:
            // WinsockError
            // 
            // On failure:
            // * WinsockError::BufferTooLong         is returned when bufferLen > INT_MAX
            // * WinsockError::BufferLengthIsZero    is returned when bufferLen == 0
            // * WinsockError::OutOfMemory           is returned when buffer allocation fails
            // * WinsockError::NotInitialized        is returned when internal socket is not created 
            //                                       (This is currently defensive code, 
            //                                       because if socket is somehow not created due to an error,
            //                                       Initialize() does not return an object)
            // * WinsockError::CheckLastWinsockError is returned when when an internal Winsock2 error occurs.
            //                                       call GetLastWinsockError() to received Error Code, then check Winsock2 documentation
            // 
            // On success:
            // WinsockError::Success                 is returned and recvByteCount is set to received byte count
            WinsockError RecvFrom(std::string& buffer, size_t bufferLen, size_t& recvByteCount, std::string& fromIpAddr, std::string& fromPort);

            // SendTo()
            // 
            // Summary:
            // Send data to specified IP Address and Port during initialization
            // Check sentByteCount to see how many bytes are sent, call this again if all data is not sent
            // 
            // Arguments:
            // const std::string& buffer    --- In
            // size_t bufferLen             --- In
            // size_t& sentByteCount        --- Out
            // const std::string& toIpAddr  --- In (Ip Address of UDP Client that data will be send)
            // const std::string& toPort    --- In (Port of UDP Client that data will be send)
            // 
            // Returns:
            // WinsockError
            // 
            // On failure:
            // * WinsockError::BufferTooLong         is returned when bufferLen > INT_MAX
            // * WinsockError::BufferLengthIsZero    is returned when bufferLen == 0
            // * WinsockError::NotInitialized        is returned when internal socket is not created 
            //                                       (This is currently defensive code, 
            //                                       because if socket is somehow not created due to an error,
            //                                       Initialize() does not return an object)
            // * WinsockError::CheckLastWinsockError is returned when when an internal Winsock2 error occurs.
            //                                       call GetLastWinsockError() to received Error Code, then check Winsock2 documentation
            // 
            // On success:
            // WinsockError::Success                 is returned and sentByteCount is set to sent byte count
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
            // Destructor: closesocket is called appropriately, do not worry about it...
            ~UdpServerCls();
        };
    }
}

#endif