#ifndef TFTPSERVERCLS_H
#define TFTPSERVERCLS_H

#include "TftpTypePkg.h"
#include "SocketPkg.h"
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
        std::unique_ptr<UtilityLib::Network::SocketServerCls> Socket;
        TftpServerCls();
        TftpServerCls(const std::string& ipAddress);

    public:
        static std::optional<TftpServerCls> Initialize(const std::string& ipAddress);
    };
}

#endif