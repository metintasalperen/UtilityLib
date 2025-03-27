#ifndef TFTPSERVERCLS_H
#define TFTPSERVERCLS_H

#include "TftpTypePkg.h"
#include "TftpPacketPkg.h"
#include "UdpServerCls.h"
#include "FilePkg.h"
#include "StringPkg.h"

#include <variant>
#include <vector>
#include <string>

namespace UtilityLib
{
    namespace Tftp
    {
        class TftpServerCls
        {
        private:
            UtilityLib::Socket::UdpServerCls UdpServer;
            std::string CurrentDirectory;

            TftpServerCls(UtilityLib::Socket::UdpServerCls&& udpServer) noexcept;
            TftpServerCls& operator=(UtilityLib::Socket::UdpServerCls&& udpServer) noexcept;

            std::vector<std::string> PrepareFileContent(const std::string& fullpath, Mode mode);


        public:
            TftpServerCls() = delete;
            TftpServerCls(const TftpServerCls&) = delete;
            TftpServerCls& operator=(const TftpServerCls&) = delete;

            TftpServerCls(TftpServerCls&& other) noexcept;
            TftpServerCls& operator=(TftpServerCls&& other) noexcept;

            static std::variant<TftpError, TftpServerCls> Initialize(const std::string& directoryPath);

            void HandleClients();
            void HandleReadRequest(RrqWrqPacketStc packet, std::string ipAddress, std::string port);
            void HandleWriteRequest(RrqWrqPacketStc packet, std::string ipAddress, std::string port);
        };
    }
}

#endif