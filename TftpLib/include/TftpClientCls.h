#ifndef TFTPCLIENTCLS_H
#define TFTPCLIENTCLS_H

#include "TftpTypePkg.h"
#include "FilePkg.h"
#include "StringPkg.h"
#include "UdpClientCls.h"
#include "TftpPacketPkg.h"

#include <cstdint>
#include <string>
#include <vector>
#include <variant>

namespace UtilityLib
{
    namespace Tftp
    {
        class TftpClientCls
        {
        private:
            UtilityLib::Socket::UdpClientCls UdpClient;
            
            TftpClientCls(UtilityLib::Socket::UdpClientCls&& other) noexcept;
            
        public:
            // Initialize()
            // 
            // Summary:
            // Initialize TFTP Client
            // 
            // Arguments:
            // const std::string& ipAddress  --- In (Ip Address of TFTP Server)
            // 
            // Returns:
            // std::variant<TftpError, TftpClientCls>
            // 
            // If initialization is successful, a new TftpClientCls object will be returned
            // If initialization is not successful,
            // TftpError::InvalidIpAddress is returned when provided IP Address is not a valid IP Address
            // TftpError::WinsockError     is returned when when an internal Winsock2 error occurs.
            //                             call GetLastWinsockError() to received Error Code, then check Winsock2 documentation
            // 
            // Note: TFTP Server IP Address can be changed after initialization via ChangeIpAddress()
            static std::variant<TftpError, TftpClientCls> Initialize(const std::string& ipAddress);
            
            // ChangeIpAddress()
            // 
            // Summary:
            // Change IP Address of the TFTP Server
            // 
            // Arguments:
            // const std::string& ipAddress  --- In (Ip Address of TFTP Server)
            // 
            // Returns:
            // TftpError
            // 
            // On success, TftpError::Success is returned
            // 
            // On failure, TftpError::InvalidIpAddress is returned when provided IP Address is not a valid IP Address
            //             TftpError::WinsockError     is returned when when an internal Winsock2 error occurs.
            //                                         call GetLastWinsockError() to received Error Code, then check Winsock2 documentation
            TftpError ChangeIpAddress(const std::string& ipAddress);
            
            // ReadFile()
            // 
            // Summary:
            // Reads specified file from TFTP server and saves to the specified location
            // 
            // Arguments:
            // const std::string& filename        --- In
            // const std::string& pathToSaveFile  --- In
            // Mode mode                          --- In
            // 
            // Returns:
            // TftpError
            // 
            // On success:
            // TftpError::Success
            // 
            // On failure:
            // TftpError::CannotSaveReadFileToDisk is returned when file cannot be saved to disk
            // TftpError::UndefinedResponse        is returned when TFTP server sends an undefined response (not defined in TFTP specification)
            // TftpError::WinsockError             is returned when when an internal Winsock2 error occurs.
            //                                     call GetLastWinsockError() to received Error Code, then check Winsock2 documentation
            TftpError ReadFile(const std::string& filename, const std::string& pathToSaveFile, Mode mode);
            
            // WriteFile()
            // NOT_IMPLEMENTED_YET
            // Summary:
            // Writes specified file to TFTP server
            // 
            // Arguments:
            // const std::string& filename    --- In
            // const std::string& pathToFile  --- In
            // Mode mode                      --- In
            // 
            // Returns:
            // TftpError
            // 
            // On success:
            // TftpError::Success
            // 
            // On failure:
            // TftpError::CannotSaveReadFileToDisk is returned when file cannot be saved to disk
            // TftpError::UndefinedResponse        is returned when TFTP server sends an undefined response (not defined in TFTP specification)
            // TftpError::WinsockError             is returned when when an internal Winsock2 error occurs.
            //                                     call GetLastWinsockError() to received Error Code, then check Winsock2 documentation
            TftpError WriteFile(const std::string& filename, const std::string& pathToFile, Mode mode);
            
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
            TftpClientCls(TftpClientCls&&) noexcept;
            // Move assignment operator
            TftpClientCls& operator=(TftpClientCls&&) noexcept;
            // Copy constructor is deleted
            TftpClientCls(const TftpClientCls&) = delete;
            // Copy assignment operator is deleted
            TftpClientCls& operator=(const TftpClientCls&) = delete;
            // Default constructor is deleted, use Initialize() to create an object to this
            TftpClientCls() = delete;
        };
    }
}

#endif