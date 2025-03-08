#include "TftpClientCls.h"

namespace Tftp
{
    TftpClientCls::TftpClientCls() :
        Socket(std::make_unique<UtilityLib::Network::SocketClientCls>())
    {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        Socket->SetHints(hints);
        Socket->SetPort(TFTP_PORT);
    }
    TftpClientCls::TftpClientCls(const std::string& ipAddress) :
        TftpClientCls()
    {
        Socket->SetIpAddress(ipAddress);
    }
    std::optional<TftpClientCls> TftpClientCls::Initialize(const std::string& ipAddress)
    {
        bool isIpAddressValid = UtilityLib::String::ValidateIpAddress(ipAddress);
        if (!isIpAddressValid) return std::nullopt;

        TftpClientCls tftpClient(ipAddress);

        ErrorEnum result = tftpClient.Socket->InitializeWinsock();
        if (result != ErrorEnum::Success)
        {
            return std::nullopt;
        }

        result = tftpClient.Socket->GetAddressInfo();
        if (result != ErrorEnum::Success)
        {
            return std::nullopt;
        }

        result = tftpClient.Socket->CreateSocket();
        if (result != ErrorEnum::Success)
        {
            return std::nullopt;
        }

        tftpClient.Socket->SetBlockingMode(UtilityLib::Network::BlockingMode::Blocking);

        return std::make_optional<TftpClientCls>(std::move(tftpClient));
    }
    bool TftpClientCls::ChangeIpAddress(const std::string& ipAddress)
    {
        bool isIpAddressValid = UtilityLib::String::ValidateIpAddress(ipAddress);
        if (!isIpAddressValid) return false;

        auto newSocket = std::make_unique<UtilityLib::Network::SocketClientCls>();

        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;

        newSocket->SetHints(hints);
        newSocket->SetPort(TFTP_PORT);
        newSocket->SetIpAddress(ipAddress);

        ErrorEnum result = newSocket->InitializeWinsock();
        if (result != ErrorEnum::Success)
        {
            return false;
        }

        result = newSocket->GetAddressInfo();
        if (result != ErrorEnum::Success)
        {
            return false;
        }

        result = newSocket->CreateSocket();
        if (result != ErrorEnum::Success)
        {
            return false;
        }

        newSocket->SetBlockingMode(UtilityLib::Network::BlockingMode::Blocking);

        Socket = std::move(newSocket);
        return true;
    }
    int TftpClientCls::GetLastWinsockError()
    {
        return Socket->GetLastWsaError();
    }

    TftpErrorCodes TftpClientCls::ReadFile(const std::string& filename, const std::string& pathToSaveFile, Mode mode)
    {
        // Send read request packet to server
        std::string readRequestPacket = CreateRrqPacket(filename, mode);
        size_t sentBytes = 0;
        ErrorEnum result = Socket->SendTo(readRequestPacket.c_str(), readRequestPacket.size(), sentBytes);
        if (result != ErrorEnum::Success) return TftpErrorCodes::WinsockError;

        std::string dataPacket;
        std::string ackPacket;
        std::string fileContent;
        size_t recvBytes = 0;
        uint16_t prevBlock = 0;

        do
        {
            // Receive data packet from server
            result = Socket->RecvFrom(dataPacket, MAX_PACKET_SIZE);
            if (result != ErrorEnum::Success) return TftpErrorCodes::WinsockError;

            recvBytes = dataPacket.size();

            auto parsedDataPacket = ParsePacket(dataPacket);
            // Check if the packet is a data packet
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
                result = Socket->SendTo(ackPacket.c_str(), ackPacket.size(), sentBytes);
                if (result != ErrorEnum::Success) return TftpErrorCodes::WinsockError;
            }
            // Check if the packet is an error packet
            else if (std::holds_alternative<Tftp::ErrorPacketStc>(parsedDataPacket) == true)
            {
                Tftp::ErrorPacketStc error = std::get<Tftp::ErrorPacketStc>(parsedDataPacket);
                return error.ErrorCode;
            }
            else
            {
                return TftpErrorCodes::UndefinedResponse;
            }
            
        } while (recvBytes == MAX_PACKET_SIZE);

        // Replace all \r\n with \n
        fileContent = UtilityLib::String::ReplaceAll(fileContent, "\r\n", "\n");

        // Save file to disk
        bool isFileWritten = UtilityLib::FileIO::WriteToFile(pathToSaveFile, fileContent);
        if (isFileWritten) return TftpErrorCodes::Success;
        else return TftpErrorCodes::CannotSaveReadFileToDisk;
    }
    TftpErrorCodes TftpClientCls::WriteFile(const std::string& filename, const std::string& pathToFile, Mode mode)
    {
        // Create full path
        std::string fullPath = pathToFile;
        bool isEndWithSlash = UtilityLib::String::IsEndWith(fullPath, "\\");
        if (!isEndWithSlash) fullPath += "\\";
        fullPath += filename;

        // If file does not exist, return error
        bool isFileExist = UtilityLib::FileIO::IsFileExist(fullPath);
        if (!isFileExist) return TftpErrorCodes::FileAtSpecifiedPathNotFound;

        // Read file content
        std::string fileContent = UtilityLib::FileIO::ReadFromFile(fullPath);
        // Replace \r\n with \n
        fileContent = UtilityLib::String::ReplaceAll(fileContent, "\r\n", "\n");

        // Divide data to 512 byte blocks
        std::vector dividedData = UtilityLib::String::DivideByLength(fileContent, MAX_DATA_SIZE);
        // If last block is 512 bytes, add an empty block
        if (dividedData.back().size() == MAX_DATA_SIZE) dividedData.push_back(std::string());

        // Send write request packet to TFTP server
        std::string writeRequestPacket = CreateWrqPacket(filename, mode);
        size_t sentBytes = 0;
        ErrorEnum result = Socket->SendTo(writeRequestPacket.c_str(), writeRequestPacket.size(), sentBytes);
        if (result != ErrorEnum::Success) return TftpErrorCodes::WinsockError;

        // Receive acknowledgment packet from server
        std::string ackPacket;
        result = Socket->RecvFrom(ackPacket, MAX_PACKET_SIZE);
        if (result != ErrorEnum::Success) return TftpErrorCodes::WinsockError;

        // Parse acknowledgment packet
        auto parsedAckPacket = ParsePacket(ackPacket);
        if (std::holds_alternative<Tftp::AckPacketStc>(parsedAckPacket) == false) return TftpErrorCodes::UndefinedResponse;
        Tftp::AckPacketStc& ack = std::get<Tftp::AckPacketStc>(parsedAckPacket);
        if (ack.Opcode != Opcode::Acknowledgment || ack.Block != 0) return TftpErrorCodes::UndefinedResponse;

        // Ack received, send data packets to server
        uint16_t block = 1;
        for (const std::string& data : dividedData)
        {
            std::string dataPacket = CreateDataPacket(block, data);
            result = Socket->SendTo(dataPacket.c_str(), dataPacket.size(), sentBytes);
            if (result != ErrorEnum::Success) return TftpErrorCodes::WinsockError;

            // Receive acknowledgment packet from server
            result = Socket->RecvFrom(ackPacket, MAX_PACKET_SIZE);
            if (result != ErrorEnum::Success) return TftpErrorCodes::WinsockError;

            // Parse acknowledgment packet
            parsedAckPacket = ParsePacket(ackPacket);
            if (std::holds_alternative<Tftp::AckPacketStc>(parsedAckPacket) == false) return TftpErrorCodes::UndefinedResponse;
            Tftp::AckPacketStc& ack = std::get<Tftp::AckPacketStc>(parsedAckPacket);
            if (ack.Opcode != Opcode::Acknowledgment) return TftpErrorCodes::UndefinedResponse;
            if (ack.Block == block) block++;
            // Else: If block number is not correct, data packet will be resend in the next iteration
        }

        return TftpErrorCodes::Success;
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
    TftpErrorCodes TftpClientCls::DetermineErrorCode(const std::string& packet)
    {
        TftpErrorCodes errorCode = TftpErrorCodes::UnknownErrorCode;

        if (static_cast<uint32_t>(packet[3]) < static_cast<uint32_t>(TftpErrorCodes::UnknownErrorCode))
        {
            errorCode = static_cast<TftpErrorCodes>(packet[3]);
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
                TftpErrorCodes errorCode = DetermineErrorCode(packet);
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