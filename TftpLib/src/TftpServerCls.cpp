#include "TftpServerCls.h"

using namespace UtilityLib::Socket;

namespace Tftp
{
    std::variant<TftpError, TftpServerCls> TftpServerCls::Initialize(const std::string& ipAddress)
    {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        auto initUdp = UdpServerCls::Initialize(hints, ipAddress, TFTP_PORT);

        if (std::holds_alternative<WinsockError>(initUdp) == true)
        {
            WinsockError& result = std::get<WinsockError>(initUdp);
            if (result == WinsockError::InvalidIpAddress)
            {
                return TftpError::InvalidIpAddress;
            }
            else
            {
                return TftpError::WinsockError;
            }
        }

        TftpServerCls tftpServer(std::move(std::get<UdpServerCls>(initUdp)));
        return tftpServer;
    }
}