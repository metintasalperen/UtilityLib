#include "TftpServerCls.h"

namespace Tftp
{
    TftpServerCls::TftpServerCls() :
        Socket(std::make_unique<UtilityLib::Network::SocketServerCls>())
    {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        Socket->SetHints(hints);
        Socket->SetPort(TFTP_PORT);
    }
    TftpServerCls::TftpServerCls(const std::string& ipAddress) :
        TftpServerCls()
    {
        Socket->SetIpAddress(ipAddress);
    }
    std::optional<TftpServerCls> TftpServerCls::Initialize(const std::string& ipAddress)
    {
        bool isIpAddressValid = UtilityLib::String::ValidateIpAddress(ipAddress);
        if (!isIpAddressValid) return std::nullopt;

        TftpServerCls tftpServer(ipAddress);

        ErrorEnum result = tftpServer.Socket->InitializeWinsock();
        if (result != ErrorEnum::Success)
        {
            return std::nullopt;
        }

        result = tftpServer.Socket->GetAddressInfo();
        if (result != ErrorEnum::Success)
        {
            return std::nullopt;
        }

        result = tftpServer.Socket->CreateSocket();
        if (result != ErrorEnum::Success)
        {
            return std::nullopt;
        }

        result = tftpServer.Socket->Bind();
        if (result != ErrorEnum::Success)
        {
            return std::nullopt;
        }

        tftpServer.Socket->SetBlockingMode(UtilityLib::Network::BlockingMode::Blocking);

        return std::make_optional<TftpServerCls>(std::move(tftpServer));
    }
}