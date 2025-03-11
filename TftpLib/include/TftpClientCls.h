#ifndef TFTPCLIENTCLS_H
#define TFTPCLIENTCLS_H

#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <memory>

#include "TftpTypePkg.h"
#include "SocketPkg.h"
#include "FilePkg.h"

namespace Tftp
{
    class TftpClientCls
    {
    private:
        UtilityLib::Network::UdpClientCls UdpClient;

    public:
        static std::variant<TftpError, TftpClientCls> Initialize(const std::string& ipAddress);
        TftpError ChangeIpAddress(const std::string& ipAddress);

        TftpError ReadFile(const std::string& filename, const std::string& pathToSaveFile, Mode mode);
        TftpError WriteFile(const std::string& filename, const std::string& pathToFile, Mode mode);
        int GetLastWinsockError();

        TftpClientCls(TftpClientCls&& other) noexcept;
        TftpClientCls& operator=(TftpClientCls&& other) noexcept;
        TftpClientCls(UtilityLib::Network::UdpClientCls&& udpClient) noexcept;

        TftpClientCls(const TftpClientCls&) = delete;
        TftpClientCls& operator=(TftpClientCls& other) = delete;
        TftpClientCls() = delete;

    private:
        Opcode DeterminePacketType(const std::string& packet);
        uint16_t DetermineBlock(const std::string& packet);
        std::string DetermineData(const std::string& packet);
        TftpError DetermineErrorCode(const std::string& packet);
        std::string DetermineErrorMessage(const std::string& packet);

        std::string CreateRrqPacket(const std::string& filename, Mode mode);
        std::string CreateWrqPacket(const std::string& filename, Mode mode);
        std::string CreateAckPacket(uint16_t block);
        std::string CreateDataPacket(uint16_t block, const std::string& data);

        std::variant<bool, DataPacketStc, AckPacketStc, ErrorPacketStc> ParsePacket(const std::string& packet);
    };
}

#endif