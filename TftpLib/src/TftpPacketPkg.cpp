#include "TftpPacketPkg.h"

namespace UtilityLib
{
    namespace Tftp
    {
        Opcode FindPacketType(const std::string& packet)
        {
            Opcode opcode = Opcode::Invalid;
        
            if (packet.size() >= MIN_PACKET_SIZE &&
                packet[0] == 0)
            {
                opcode = static_cast<Opcode>(packet[1]);
        
                if (opcode < Opcode::ReadRequest ||
                    opcode > Opcode::Error)
                {
                    opcode = Opcode::Invalid;
                }
            }
        
            return opcode;
        }
        uint16_t ExtractBlock(const std::string& packet)
        {
            const uint16_t* ptr = reinterpret_cast<const uint16_t*>(packet.c_str() + 2);
            uint16_t result = ntohs(*ptr);
            return result;
        }
        std::string ExtractData(const std::string& packet)
        {
            if (packet.size() >= MIN_PACKET_SIZE)
            {
                return packet.substr(4, packet.size() - 4);
            }
            else
            {
                return std::string();
            }
        }
        TftpError ExtractErrorCode(const std::string& packet)
        {
            TftpError errorCode = TftpError::UnknownErrorCode;
        
            if (static_cast<uint32_t>(packet[3]) < static_cast<uint32_t>(TftpError::UnknownErrorCode))
            {
                errorCode = static_cast<TftpError>(packet[3]);
            }
        
            return errorCode;
        }
        std::string ExtractErrorMessage(const std::string& packet)
        {
            std::string errorMessage;
        
            if (packet.size() > MIN_PACKET_SIZE)
            {
                errorMessage = packet.substr(4, packet.size() - 4);
            }
        
            return errorMessage;
        }
        std::string ExtractFileName(const std::string& packet)
        {
            size_t firstNullTerminator = packet.find_first_of('\0');
            std::string filename = packet.substr(2, firstNullTerminator - 2);
            return filename;
        }
        Mode ExtractMode(const std::string& packet)
        {
            size_t firstNullTerminator = packet.find_first_of('\0');
            std::string mode = UtilityLib::String::ToLower(packet.substr(firstNullTerminator + 1, packet.size() - firstNullTerminator + 1));
        
            size_t findIndex = mode.find("octet");
            if (findIndex != std::string::npos)
            {
                return Mode::Octet;
            }
            findIndex = mode.find("netascii");
            if (findIndex != std::string::npos)
            {
                return Mode::NetAscii;
            }
            findIndex = mode.find('@');
            if (findIndex != std::string::npos)
            {
                return Mode::Mail;
            }
            return Mode::Invalid;
        }
        
        std::string CreateRrqPacket(const std::string& filename, Mode mode, size_t& packetSize)
        {
            const size_t nullTerminatorCount = 2;
        
            std::string modeName = MODE_MAPPING.at(mode);
            size_t filenameSize = filename.size();
            size_t modeSize = modeName.size();
        
            packetSize = OPCODE_SIZE + filenameSize + modeSize + nullTerminatorCount;
        
            std::string packet(packetSize, '\0');
        
            // Opcode
            packet[0] = 0;
            packet[1] = static_cast<char>(Opcode::ReadRequest);
        
            // Filename
            for (size_t i = 0; i < filenameSize; i++)
            {
                packet[i + 2] = filename[i];
            }
        
            // Null terminator
            packet[OPCODE_SIZE + filenameSize] = '\0';
        
            // Mode
            for (size_t i = 0; i < modeSize; i++)
            {
                packet[OPCODE_SIZE + filenameSize + 1 + i] = modeName[i];
            }
        
            // Null terminator
            packet[packetSize - 1] = '\0';
        
            return packet;
        }
        std::string CreateWrqPacket(const std::string& filename, Mode mode, size_t& packetSize)
        {
            const size_t nullTerminatorCount = 2;
        
            std::string modeName = MODE_MAPPING.at(mode);
            size_t filenameSize = filename.size();
            size_t modeSize = modeName.size();
        
            packetSize = OPCODE_SIZE + filenameSize + modeSize + nullTerminatorCount;
        
            std::string packet(packetSize, '\0');
        
            // Opcode
            packet[0] = 0;
            packet[1] = static_cast<char>(Opcode::WriteRequest);
        
            // Filename
            for (size_t i = 0; i < filenameSize; i++)
            {
                packet[i + 2] = filename[i];
            }
        
            // Null terminator
            packet[OPCODE_SIZE + filenameSize] = '\0';
        
            // Mode
            for (size_t i = 0; i < modeSize; i++)
            {
                packet[OPCODE_SIZE + filenameSize + 1 + i] = modeName[i];
            }
        
            // Null terminator
            packet[packetSize - 1] = '\0';
        
            return packet;
        }
        std::string CreateAckPacket(uint16_t block, size_t& packetSize)
        {
            packetSize = OPCODE_SIZE + BLOCK_SIZE;
            std::string ackPacket(packetSize, '\0');
            ackPacket[1] = static_cast<char>(Opcode::Acknowledgment);
        
            ackPacket[2] = static_cast<char>((block >> 8) & 0xFF);
            ackPacket[3] = static_cast<char>(block & 0xFF);
        
            return ackPacket;
        }
        std::string CreateDataPacket(uint16_t block, const std::string& data, size_t& packetSize)
        {
            std::string dataPacket = CreateAckPacket(block, packetSize);
            dataPacket[1] = static_cast<char>(Opcode::Data);
        
            dataPacket += data;
            packetSize += data.size();
        
            return dataPacket;
        }
        
        std::variant<bool, DataPacketStc, AckPacketStc, ErrorPacketStc, RrqWrqPacketStc> Tftp::ParsePacket(const std::string& packet)
        {
            if (packet.size() < MIN_PACKET_SIZE)
            {
                return false;
            }
        
            Opcode opcode = FindPacketType(packet);
        
            switch (opcode)
            {
                case Opcode::Acknowledgment:
                {
                    uint16_t block = ExtractBlock(packet);
                    AckPacketStc ackPkg = { opcode, block };
                
                    return ackPkg;
                }
                case Opcode::Data:
                {
                    uint16_t block = ExtractBlock(packet);
                    std::string data = ExtractData(packet);
                    DataPacketStc dataPkg = { opcode, block, data };
                
                    return dataPkg;
                }
                case Opcode::Error:
                {
                    TftpError errorCode = ExtractErrorCode(packet);
                    std::string errorMessage = ExtractErrorMessage(packet);
                    ErrorPacketStc errorPkg = { opcode, errorCode, errorMessage };
                
                    return errorPkg;
                }
                case Opcode::ReadRequest:
                case Opcode::WriteRequest:
                {
                    std::string filename = ExtractFileName(packet);
                    Mode mode = ExtractMode(packet);
                    RrqWrqPacketStc rrqWrqPkg = { opcode, filename, mode };
                
                    return rrqWrqPkg;
                }
                default:
                {
                    return false;
                }
            }
        }
        
        uint16_t GenerateRandomPort()
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<uint16_t> dis(20000, USHRT_MAX);
            return dis(gen);
        }
    }
}