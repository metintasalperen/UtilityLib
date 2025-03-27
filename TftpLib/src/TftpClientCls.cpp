#include "TftpClientCls.h"

using namespace UtilityLib::Socket;

namespace UtilityLib
{
    namespace Tftp
    {
        TftpClientCls::TftpClientCls(UdpClientCls&& other) noexcept :
            UdpClient(std::move(other))
        {
        }
        TftpClientCls::TftpClientCls(TftpClientCls&& other) noexcept :
            UdpClient(std::move(other.UdpClient))
        {
        }
        TftpClientCls& TftpClientCls::operator=(TftpClientCls&& other) noexcept
        {
            UdpClient = std::move(other.UdpClient);
            return *this;
        }

        std::variant<TftpError, TftpClientCls> TftpClientCls::Initialize(const std::string& ipAddress)
        {
            auto initUdp = UdpClientCls::Initialize(ipAddress, TFTP_PORT);

            if (std::holds_alternative<WinsockError>(initUdp))
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
        TftpError TftpClientCls::ChangeIpAddress(const std::string& ipAddress)
        {
            auto initUdp = UdpClientCls::Initialize(ipAddress, TFTP_PORT);

            if (std::holds_alternative<WinsockError>(initUdp))
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
            size_t packetSize = 0;
            size_t sentBytes = 0;
            size_t recvBytes = MAX_PACKET_SIZE;
            uint16_t prevBlock = 0;

            std::string fromIpAddr, fromPort;
            std::string fileContent;
            std::string dataPacket, ackPacket;

            std::string fullpath = UtilityLib::FileIO::CreateFullPath(filename, pathToSaveFile);
            std::ofstream fileStream = UtilityLib::FileIO::OpenFile(fullpath, UtilityLib::FileIO::FileMode::WriteBinaryAppend);

            // Create Read Request packet
            std::string readRequestPacket = CreateRrqPacket(filename, mode, packetSize);

            // Send Read Request to TFTP Server
            WinsockError result = UdpClient.SendTo(readRequestPacket, packetSize, sentBytes);
            if (result != WinsockError::Success)
            {
                return TftpError::WinsockError;
            }

            bool firstIteration = true;
            while (recvBytes == MAX_PACKET_SIZE)
            {
                if (firstIteration == false) [[likely]]
                {
                    result = UdpClient.RecvFrom(dataPacket, MAX_PACKET_SIZE, recvBytes);
                    if (result != WinsockError::Success)
                    {
                        return TftpError::WinsockError;
                    }
                }
                else [[unlikely]]
                    {
                        result = UdpClient.RecvFrom(dataPacket, MAX_PACKET_SIZE, recvBytes, fromIpAddr, fromPort);
                        if (result != WinsockError::Success)
                        {
                            return TftpError::WinsockError;
                        }

                        // Server will accept initial requests through port 69, then will randomly select a new port to continue file transfer
                        // Change Port to the new port selected by server
                        UdpClient.SetPort(fromPort);
                        firstIteration = false;
                    }

                    auto parsedDataPacket = ParsePacket(dataPacket);
                    // Data packet received
                    if (std::holds_alternative<DataPacketStc>(parsedDataPacket))
                    {
                        DataPacketStc& data = std::get<DataPacketStc>(parsedDataPacket);

                        if (prevBlock + 1 == data.Block)
                        {
                            if (UtilityLib::FileIO::WriteToFile(fileStream, data.Data) == false)
                            {
                                return TftpError::CannotSaveReadFileToDisk;
                            }

                            dataPacket.clear();
                            prevBlock = data.Block;
                        }

                        ackPacket = CreateAckPacket(data.Block, packetSize);
                        result = UdpClient.SendTo(ackPacket, packetSize, sentBytes);
                        if (result != WinsockError::Success)
                        {
                            return TftpError::WinsockError;
                        }
                    }
                    // Error packet received
                    else if (std::holds_alternative<ErrorPacketStc>(parsedDataPacket))
                    {
                        ErrorPacketStc& error = std::get<ErrorPacketStc>(parsedDataPacket);
                        return error.ErrorCode;
                    }
                    // Unknown packet received
                    else
                    {
                        return TftpError::UndefinedResponse;
                    }
            }

            fileStream.close();

            return TftpError::Success;
        }
        TftpError TftpClientCls::WriteFile(const std::string& filename, const std::string& pathToFile, Mode mode)
        {
            size_t packetSize = 0;
            size_t sentBytes = 0;
            size_t recvBytes = 0;
            std::string ackPacket;
            std::string fromIpAddr, fromPort;

            // Read file that will be send
            std::string fullpath = UtilityLib::FileIO::CreateFullPath(filename, pathToFile);
            bool isFileExist = UtilityLib::FileIO::IsFileExist(fullpath);
            if (!isFileExist) return TftpError::FileAtSpecifiedPathNotFound;
            std::string fileContent = UtilityLib::FileIO::ReadFromFile(fullpath);

            // Divide data into 512 bytes blocks
            std::vector dividedData = UtilityLib::String::DivideByLength(fileContent, MAX_DATA_SIZE);
            // If last packet is 512 bytes, add an empty packet to end file transmission
            if (dividedData.at(dividedData.size() - 1).size() == MAX_DATA_SIZE)
            {
                dividedData.push_back(std::string());
            }

            // Send Write Request to server
            std::string writeRequestPacket = CreateWrqPacket(filename, mode, packetSize);
            WinsockError result = UdpClient.SendTo(writeRequestPacket, writeRequestPacket.size(), sentBytes);
            if (result != WinsockError::Success) return TftpError::WinsockError;

            // Read response from server
            result = UdpClient.RecvFrom(ackPacket, MAX_PACKET_SIZE, recvBytes, fromIpAddr, fromPort);
            auto parsedAckPacket = ParsePacket(ackPacket);
            if (std::holds_alternative<AckPacketStc>(parsedAckPacket) == false) return TftpError::UndefinedResponse;

            AckPacketStc& ack = std::get<AckPacketStc>(parsedAckPacket);
            if (ack.Block != 0) return TftpError::UndefinedResponse;

            // Change port from 69 to whatever server decided to continue sending
            UdpClient.SetPort(fromPort);

            // Ack received successfully, start to send files
            uint16_t block = 1;
            for (size_t i = 0; i < dividedData.size(); i++)
            {
                std::string dataPacket = CreateDataPacket(block, dividedData.at(i), packetSize);
                result = UdpClient.SendTo(dataPacket, dataPacket.size(), sentBytes);
                if (result != WinsockError::Success) return TftpError::WinsockError;

                result = UdpClient.RecvFrom(ackPacket, ACK_PACKET_SIZE, recvBytes, fromIpAddr, fromPort);
                if (result != WinsockError::Success) return TftpError::WinsockError;

                parsedAckPacket = ParsePacket(ackPacket);
                if (std::holds_alternative<AckPacketStc>(parsedAckPacket) == false) return TftpError::UndefinedResponse;

                AckPacketStc& ack = std::get<AckPacketStc>(parsedAckPacket);
                if (block == ack.Block)
                {
                    block++;
                }
                else
                {
                    return TftpError::UndefinedResponse;
                }
            }

            return TftpError::Success;
        }
    }
}