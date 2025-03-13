#ifndef TFTPSERVERCLS_H
#define TFTPSERVERCLS_H

#include "TftpTypePkg.h"
#include "UdpServerCls.h"
#include "FilePkg.h"
#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <memory>

namespace Tftp
{
    class TftpServerCls
    {
    private:
        UtilityLib::Socket::UdpServerCls UdpServer;
        TftpServerCls(UtilityLib::Socket::UdpServerCls&& udpServer) noexcept;

    public:
        TftpServerCls() = delete;
        TftpServerCls(const TftpServerCls&) = delete;
        TftpServerCls& operator=(TftpServerCls& other) = delete;

        TftpServerCls(TftpServerCls&& other) noexcept;
        TftpServerCls& operator=(TftpServerCls&& other) noexcept;

        static std::variant<TftpError, TftpServerCls> Initialize(const std::string& ipAddress);
        TftpError ChangeIpAddress(const std::string& ipAddress);


    };
}

#endif