#ifndef TCPSERVERCLS_H
#define TCPSERVERCLS_H

#include "StringPkg.h"
#include "SocketTypePkg.h"
#include "TcpSessionCls.h"

#include <variant>

namespace UtilityLib
{
    namespace Socket
    {
        class TcpServerCls
        {
        private:
            SOCKET Sock;
            std::string Port;
            int LastWinsockError;

            TcpServerCls();

            bool SetPort(const std::string& port);

            WinsockError GetAddressInfo();
            WinsockError CreateSocket();
            WinsockError CloseSocket();
            WinsockError Bind();
            WinsockError Listen(int backlog = SOMAXCONN);

        public:
            // Initialize
            // 
            // Summary:
            // Initialize as socket to use as a TCP Server
            // 
            // Arguments: 
            // const std::string& port       --- In
            // BlockingMode mode             --- In (default BlockingMode::Blocking)
            // int backlog                   --- In (default SOMAXCONN)
            // 
            // Returns:
            // std::variant<WinsockError, TcpServerCls>
            // 
            // If initialization is successful, a TcpServerCls object will be returned
            // 
            // If initialization is not successful,
            // WinsockError::InvalidPort           is returned when provided Port is not valid
            // WinsockError::CheckLastWinsockError is returned when an internal Winsock2 error occurs.
            //                                     call GetLastWinsockError() to receive error code, then check Microsoft's documentation
            // 
            // Note: Once initialized, Port cannot be changed
            // Create a new object by calling this method if you need to change it...
            static std::variant<WinsockError, TcpServerCls> Initialize(
                const std::string& port,
                BlockingMode mode = BlockingMode::Blocking,
                int backlog = SOMAXCONN);

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

            // Accept()
            // 
            // Summary:
            // Accepts incoming connections from any TCP Clients
            // Returns a TcpSessionCls object so that you can handle the connection
            // while TcpServerCls goes back to listening for new connections
            // 
            // Returns:
            // std::variant<WinsockError, TcpSessionCls>
            // 
            // On failure, returns WinsockError::CheckLastWinsockError
            //             call GetLastWinsockError() to receive error code, then check Microsoft's documentation
            // On success, returns a TcpSessionCls object that can handle connection to TCP Client
            std::variant<WinsockError, TcpSessionCls> Accept();

            // Move constructor
            TcpServerCls(TcpServerCls&& other) noexcept;
            // Move assignment operator
            TcpServerCls& operator=(TcpServerCls&& other) noexcept;
            // Copy constructor is deleted
            TcpServerCls(const TcpServerCls&) = delete;
            // Copy assignment operator is deleted
            TcpServerCls& operator=(const TcpServerCls&) = delete;
            // Destructor: closesocket is called appropriately, do not worry..
            ~TcpServerCls();
        };
    }
}

#endif