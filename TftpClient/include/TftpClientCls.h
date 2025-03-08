#ifndef TFTPCLIENTCLS_H
#define TFTPCLIENTCLS_H

#include "TftpClientTypePkg.h"
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
    struct DataPacketStc
    {
        Opcode Opcode;
        uint16_t Block;
        std::string Data;
    };

    struct AckPacketStc
    {
        Opcode Opcode;
        uint16_t Block;
    };

    struct ErrorPacketStc
    {
        Opcode Opcode;
        TftpErrorCodes ErrorCode;
        std::string ErrorMessage;
    };

    class TftpClientCls
    {
    private:
        std::unique_ptr<UtilityLib::Network::SocketClientCls> Socket;
        TftpClientCls();
        TftpClientCls(const std::string& ipAddress);

    public:
        static std::optional<TftpClientCls> Initialize(const std::string& ipAddress);
        bool ChangeIpAddress(const std::string& ipAddress);

        TftpErrorCodes ReadFile(const std::string& filename, const std::string& pathToSaveFile, Mode mode);
        TftpErrorCodes WriteFile(const std::string& filename, const std::string& pathToFile, Mode mode);
        int GetLastWinsockError();

        TftpClientCls(TftpClientCls&&) noexcept = default;
        TftpClientCls& operator=(TftpClientCls&&) noexcept = default;
        
        TftpClientCls(const TftpClientCls&) = delete;
        TftpClientCls& operator=(const TftpClientCls&) = delete;

    private:
        Opcode DeterminePacketType(const std::string& packet);
        uint16_t DetermineBlock(const std::string& packet);
        std::string DetermineData(const std::string& packet);
        TftpErrorCodes DetermineErrorCode(const std::string& packet);
        std::string DetermineErrorMessage(const std::string& packet);

        std::string CreateRrqPacket(const std::string& filename, Mode mode);
        std::string CreateWrqPacket(const std::string& filename, Mode mode);
        std::string CreateAckPacket(uint16_t block);
        std::string CreateDataPacket(uint16_t block, const std::string& data);

        std::variant<bool, DataPacketStc, AckPacketStc, ErrorPacketStc> ParsePacket(const std::string& packet);
    };
}

#endif