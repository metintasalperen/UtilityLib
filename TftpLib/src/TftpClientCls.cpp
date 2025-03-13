#include "TftpClientCls.h"

using namespace UtilityLib::Socket;

namespace Tftp
{
    std::variant<TftpError, TftpClientCls> TftpClientCls::Initialize(const std::string& ipAddress)
    {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        auto initUdp = UdpClientCls::Initialize(hints, ipAddress, TFTP_PORT);

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

        TftpClientCls tftpClient(std::move(std::get<UdpClientCls>(initUdp)));
        return tftpClient;
    }
    TftpClientCls::TftpClientCls(UdpClientCls&& udpClient) noexcept :
        UdpClient(std::move(udpClient))
    { }
    TftpClientCls::TftpClientCls(TftpClientCls&& other) noexcept :
        UdpClient(std::move(other.UdpClient))
    { }
    TftpClientCls& TftpClientCls::operator=(TftpClientCls&& other) noexcept
    {
        UdpClient = std::move(other.UdpClient);
        return *this;
    }
    TftpError TftpClientCls::ChangeIpAddress(const std::string& ipAddress)
    {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        auto initUdp = UdpClientCls::Initialize(hints, ipAddress, TFTP_PORT);

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

        UdpClient = std::move(std::get<UdpClientCls>(initUdp));
        return TftpError::Success;
    }
    int TftpClientCls::GetLastWinsockError()
    {
        return UdpClient.GetLastWinsockError();
    }

    TftpError TftpClientCls::ReadFile(const std::string& filename, const std::string& pathToSaveFile, Mode mode)
    {
        std::string readRequestPacket = CreateRrqPacket(filename, mode);
        size_t sentBytes = 0;
        WinsockError result = UdpClient.SendTo(readRequestPacket.c_str(), readRequestPacket.size(), sentBytes);
        if (result != WinsockError::Success)
        {
            return TftpError::WinsockError;
        }

        std::string dataPacket;
        std::string ackPacket;
        std::string fileContent;
        size_t recvBytes = 0;
        uint16_t prevBlock = 0;

        do
        {
            result = UdpClient.RecvFrom(dataPacket, MAX_PACKET_SIZE, recvBytes);
            if (result != WinsockError::Success)
            {
                return TftpError::WinsockError;
            }

            auto parsedDataPacket = ParsePacket(dataPacket);
            if (std::holds_alternative<Tftp::DataPacketStc>(parsedDataPacket) == true)
            {
                Tftp::DataPacketStc& data = std::get<Tftp::DataPacketStc>(parsedDataPacket);

                if (prevBlock + 1 == data.Block)
                {
                    fileContent = fileContent + data.Data;
                    dataPacket.clear();
                    prevBlock = data.Block;
                }
                
                ackPacket = CreateAckPacket(data.Block);
                result = UdpClient.SendTo(ackPacket.c_str(), ackPacket.size(), sentBytes);
                if (result != WinsockError::Success)
                {
                    return TftpError::WinsockError;
                }
            }
            else if (std::holds_alternative<Tftp::ErrorPacketStc>(parsedDataPacket) == true)
            {
                Tftp::ErrorPacketStc error = std::get<Tftp::ErrorPacketStc>(parsedDataPacket);
                return error.ErrorCode;
            }
            else
            {
                return TftpError::UndefinedResponse;
            }
            
        } while (recvBytes == MAX_PACKET_SIZE);

        // Replace all \r\n with \n
        fileContent = UtilityLib::String::ReplaceAll(fileContent, "\r\n", "\n");

        // Save file to disk
        bool isFileWritten = UtilityLib::FileIO::WriteToFile(pathToSaveFile, fileContent);
        if (isFileWritten)
        {
            return TftpError::Success;
        }
        else
        {
            return TftpError::CannotSaveReadFileToDisk;
        }
    }
    TftpError TftpClientCls::WriteFile(const std::string& filename, const std::string& pathToFile, Mode mode)
    {
        std::string fullPath = pathToFile;
        bool isEndWithSlash = UtilityLib::String::IsEndWith(fullPath, "\\");
        if (!isEndWithSlash) fullPath += "\\";
        fullPath += filename;

        bool isFileExist = UtilityLib::FileIO::IsFileExist(fullPath);
        if (!isFileExist)
        {
            return TftpError::FileAtSpecifiedPathNotFound;
        }

        std::string fileContent = UtilityLib::FileIO::ReadFromFile(fullPath);
        fileContent = UtilityLib::String::ReplaceAll(fileContent, "\r\n", "\n");
        std::vector dividedData = UtilityLib::String::DivideByLength(fileContent, MAX_DATA_SIZE);
        if (dividedData.back().size() == MAX_DATA_SIZE)
        {
            dividedData.push_back(std::string());
        }

        std::string writeRequestPacket = CreateWrqPacket(filename, mode);
        size_t sentBytes = 0;
        WinsockError result = UdpClient.SendTo(writeRequestPacket.c_str(), writeRequestPacket.size(), sentBytes);
        if (result != WinsockError::Success)
        {
            return TftpError::WinsockError;
        }

        std::string ackPacket;
        size_t recvBytes = 0;
        result = UdpClient.RecvFrom(ackPacket, MAX_PACKET_SIZE, recvBytes);
        if (result != WinsockError::Success)
        {
            return TftpError::WinsockError;
        }

        auto parsedAckPacket = ParsePacket(ackPacket);
        if (std::holds_alternative<Tftp::AckPacketStc>(parsedAckPacket) == false)
        {
            return TftpError::UndefinedResponse;
        }

        Tftp::AckPacketStc& ack = std::get<Tftp::AckPacketStc>(parsedAckPacket);
        if (ack.Opcode != Opcode::Acknowledgment || ack.Block != 0)
        {
            return TftpError::UndefinedResponse;
        }

        uint16_t block = 1;
        for (const std::string& data : dividedData)
        {
            std::string dataPacket = CreateDataPacket(block, data);
            result = UdpClient.SendTo(dataPacket.c_str(), dataPacket.size(), sentBytes);
            if (result != WinsockError::Success)
            {
                return TftpError::WinsockError;
            }

            result = UdpClient.RecvFrom(ackPacket, MAX_PACKET_SIZE, recvBytes);
            if (result != WinsockError::Success)
            {
                return TftpError::WinsockError;
            }

            parsedAckPacket = ParsePacket(ackPacket);
            if (std::holds_alternative<Tftp::AckPacketStc>(parsedAckPacket) == false)
            {
                return TftpError::UndefinedResponse;
            }
            Tftp::AckPacketStc& ack = std::get<Tftp::AckPacketStc>(parsedAckPacket);
            if (ack.Block == block)
            {
                block++;
            }
            // Else: If block number is not correct, data packet will be resend in the next iteration
        }

        return TftpError::Success;
    }

    std::string TftpClientCls::CreateRrqPacket(const std::string& filename, Mode mode)
    {
        const size_t nullTerminatorCount = 2;

        std::string modeName = MODE_MAPPING.at(mode);
        size_t filenameSize = filename.size();
        size_t modeSize = modeName.size();

        size_t packetSize = OPCODE_SIZE + filenameSize + modeSize + nullTerminatorCount;

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
    std::string TftpClientCls::CreateWrqPacket(const std::string& filename, Mode mode)
    {
        const size_t nullTerminatorCount = 2;

        std::string modeName = MODE_MAPPING.at(mode);
        size_t filenameSize = filename.size();
        size_t modeSize = modeName.size();

        size_t packetSize = OPCODE_SIZE + filenameSize + modeSize + nullTerminatorCount;

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
    std::string TftpClientCls::CreateAckPacket(uint16_t block)
    {
        std::string ackPacket(ACK_PACKET_SIZE, '\0');
        ackPacket[1] = static_cast<char>(Opcode::Acknowledgment);

        ackPacket[2] = static_cast<char>(block & 0xFF);
        ackPacket[3] = static_cast<char>((block >> 8) & 0xFF);

        return ackPacket;
    }
    std::string TftpClientCls::CreateDataPacket(uint16_t block, const std::string& data)
    {
        std::string dataPacket(MIN_PACKET_SIZE, '\0');
        dataPacket[1] = static_cast<char>(Opcode::Data);

        dataPacket[2] = static_cast<char>(block & 0xFF);
        dataPacket[3] = static_cast<char>((block >> 8) & 0xFF);

        dataPacket += data;

        return dataPacket;
    }

    Opcode TftpClientCls::DeterminePacketType(const std::string& packet)
    {
        Opcode opcode = static_cast<Opcode>(packet[1]);

        if (opcode < Opcode::ReadRequest || 
            opcode > Opcode::Error ||
            packet[0] != 0)
        {
            return Opcode::Invalid;
        }

        return opcode;
    }
    uint16_t TftpClientCls::DetermineBlock(const std::string& packet)
    {
        uint16_t msb = static_cast<uint16_t>(packet[2]);
        uint16_t lsb = static_cast<uint16_t>(packet[3]);
        return 256 * msb + lsb;
    }
    std::string TftpClientCls::DetermineData(const std::string& packet)
    {
        return packet.substr(4, packet.size() - 4);
    }
    TftpError TftpClientCls::DetermineErrorCode(const std::string& packet)
    {
        TftpError errorCode = TftpError::UnknownErrorCode;

        if (static_cast<uint32_t>(packet[3]) < static_cast<uint32_t>(TftpError::UnknownErrorCode))
        {
            errorCode = static_cast<TftpError>(packet[3]);
        }

        return errorCode;
    }
    std::string TftpClientCls::DetermineErrorMessage(const std::string& packet)
    {
        std::string errorMessage;

        if (packet.size() > 5)
        {
            errorMessage = packet.substr(4, packet.size() - 5);
        }
        
        return errorMessage;
    }

    std::variant<bool, DataPacketStc, AckPacketStc, ErrorPacketStc> TftpClientCls::ParsePacket(const std::string& packet)
    {
        if (packet.size() < MIN_PACKET_SIZE)
        {
            return false;
        }

        Opcode opcode = DeterminePacketType(packet);
        
        switch (opcode)
        {
            case Opcode::Acknowledgment:
            {
                uint16_t block = DetermineBlock(packet);
                AckPacketStc ackPkg = { opcode, block };

                return ackPkg;
            }
            case Opcode::Data:
            {
                uint16_t block = DetermineBlock(packet);
                std::string data = DetermineData(packet);
                DataPacketStc dataPkg = { opcode, block, data };

                return dataPkg;
            }
            case Opcode::Error:
            {
                TftpError errorCode = DetermineErrorCode(packet);
                std::string errorMessage = DetermineErrorMessage(packet);
                ErrorPacketStc errorPkg = { opcode, errorCode, errorMessage };

                return errorPkg;
            }
            default:
            {
                return false;
            }
        }
    }
}