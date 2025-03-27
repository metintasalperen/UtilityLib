#ifndef TFTPPACKETPKG_H
#define TFTPPACKETPKG_H

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

#include "TftpTypePkg.h"
#include "StringPkg.h"

#include <variant>
#include <random>

namespace UtilityLib
{
    namespace Tftp
    {
        Opcode FindPacketType(const std::string& packet);
        uint16_t ExtractBlock(const std::string& packet);
        std::string ExtractData(const std::string& packet);
        TftpError ExtractErrorCode(const std::string& packet);
        std::string ExtractErrorMessage(const std::string& packet);
        
        std::string CreateRrqPacket(const std::string& filename, Mode mode, size_t& packetSize);
        std::string CreateWrqPacket(const std::string& filename, Mode mode, size_t& packetSize);
        std::string CreateAckPacket(uint16_t block, size_t& packetSize);
        std::string CreateDataPacket(uint16_t block, const std::string& data, size_t& packetSize);
        
        std::variant<bool, DataPacketStc, AckPacketStc, ErrorPacketStc, RrqWrqPacketStc> ParsePacket(const std::string& packet);
        
        uint16_t GenerateRandomPort();
    }
}

#endif