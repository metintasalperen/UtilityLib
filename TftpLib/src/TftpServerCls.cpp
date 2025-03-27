#include "TftpServerCls.h"
#include <thread>

using namespace UtilityLib::Socket;

namespace UtilityLib
{
    namespace Tftp
    {
        TftpServerCls::TftpServerCls(UdpServerCls&& udpServer) noexcept :
            UdpServer(std::move(udpServer))
        {
        }
        TftpServerCls& TftpServerCls::operator=(UdpServerCls&& udpServer) noexcept
        {
            UdpServer = std::move(udpServer);
            return *this;
        }
        TftpServerCls::TftpServerCls(TftpServerCls&& other) noexcept :
            UdpServer(std::move(other.UdpServer)),
            CurrentDirectory(std::move(other.CurrentDirectory))
        {
        }
        TftpServerCls& TftpServerCls::operator=(TftpServerCls&& other) noexcept
        {
            if (this != &other)
            {
                UdpServer = std::move(other.UdpServer);
                CurrentDirectory = std::move(other.CurrentDirectory);
            }
            return *this;
        }

        std::variant<TftpError, TftpServerCls> TftpServerCls::Initialize(const std::string& directoryPath)
        {
            auto initUdp = UdpServerCls::Initialize(TFTP_PORT);

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

            TftpServerCls tftpServer(std::move(std::get<UdpServerCls>(initUdp)));
            tftpServer.CurrentDirectory = directoryPath;
            return tftpServer;
        }

        void TftpServerCls::HandleClients()
        {
            while (true)
            {
                std::string buffer;
                size_t bufferLen = MAX_PACKET_SIZE;
                size_t recvByteCount = 0;
                std::string clientIp, clientPort;

                WinsockError result = UdpServer.RecvFrom(buffer, bufferLen, recvByteCount, clientIp, clientPort);

                if (result != WinsockError::Success)
                    continue;

                auto parsedPacket = ParsePacket(buffer);
                if (std::holds_alternative<RrqWrqPacketStc>(parsedPacket) == false)
                    continue;

                RrqWrqPacketStc packet = std::get<RrqWrqPacketStc>(parsedPacket);

                if (packet.Opcode == Opcode::ReadRequest)
                {
                    std::thread t(&TftpServerCls::HandleReadRequest, this, packet, clientIp, clientPort);
                    t.detach();
                }
                else if (packet.Opcode == Opcode::WriteRequest)
                {
                    std::thread t(&TftpServerCls::HandleWriteRequest, this, packet, clientIp, clientPort);
                    t.detach();
                }
                else
                {
                    std::string errMsg(4, '\0');
                    errMsg[1] = static_cast<char>(Opcode::Error);
                    errMsg[3] = static_cast<char>(TftpError::IllegalTftpOperation);
                    errMsg += "Invalid TFTP Operation";

                    size_t sentByteCount = 0;
                    UdpServer.SendTo(errMsg, errMsg.size(), sentByteCount, clientIp, clientPort);
                }
            }
        }

        void TftpServerCls::HandleReadRequest(RrqWrqPacketStc packet, std::string ipAddress, std::string port)
        {
            auto udpServerInit = UdpServerCls::Initialize(port, BlockingMode::Blocking, ipAddress);
            if (std::holds_alternative<WinsockError>(udpServerInit))
                return;

            UdpServerCls& udpServer = std::get<UdpServerCls>(udpServerInit);

            std::string fullpath = UtilityLib::FileIO::CreateFullPath(packet.Filename, CurrentDirectory);
            std::vector<std::string> dividedFileContent = PrepareFileContent(fullpath, packet.Mode);

            uint16_t block = 1;
            size_t packetSize = 0;
            size_t sentBytes = 0;
            size_t recvBytes = 0;
            std::string dataPacket;
            std::string ackPacket;

            int errCode = 0;

            for (size_t i = 0; i < dividedFileContent.size(); i++)
            {
                dataPacket = CreateDataPacket(block, dividedFileContent.at(i), packetSize);

                WinsockError result = udpServer.SendTo(dataPacket, packetSize, sentBytes, ipAddress, port);
                if (result != WinsockError::Success) break;

                result = udpServer.RecvFrom(ackPacket, MAX_PACKET_SIZE, recvBytes, ipAddress, port);
                if (result != WinsockError::Success)
                {
                    if (result == WinsockError::CheckLastWinsockError)
                        errCode = udpServer.GetLastWinsockError();
                    break;
                }

                auto parsedAckPacket = ParsePacket(ackPacket);
                if (std::holds_alternative<AckPacketStc>(parsedAckPacket) == false)
                    break;
                AckPacketStc& ack = std::get<AckPacketStc>(parsedAckPacket);

                if (ack.Block == block)
                    block++;
                else
                    break;
            }
        }

        void TftpServerCls::HandleWriteRequest(RrqWrqPacketStc packet, std::string ipAddress, std::string port)
        {
            auto udpServerInit = UdpServerCls::Initialize(port, BlockingMode::Blocking, ipAddress);
            if (std::holds_alternative<WinsockError>(udpServerInit))
                return;

            UdpServerCls& udpServer = std::get<UdpServerCls>(udpServerInit);

            std::ofstream file;
            std::string fullpath = UtilityLib::FileIO::CreateFullPath(packet.Filename, CurrentDirectory);
            std::string filePart;
            std::string ackPacket, dataPacket;
            uint16_t block = 0;
            size_t recvBlockSize = MAX_PACKET_SIZE;
            size_t packetSize = 0;
            size_t sentByteCount, recvByteCount;

            if (packet.Mode == Mode::Octet)
            {
                file = UtilityLib::FileIO::OpenFile(fullpath, UtilityLib::FileIO::FileMode::WriteBinaryAppend);
            }
            else if (packet.Mode == Mode::NetAscii)
            {
                file = UtilityLib::FileIO::OpenFile(fullpath, UtilityLib::FileIO::FileMode::WriteTextAppend);
            }
            else
            {
                return;
            }


            while (recvBlockSize == MAX_PACKET_SIZE)
            {
                ackPacket = CreateAckPacket(block, packetSize);
                if (udpServer.SendTo(ackPacket, packetSize, sentByteCount, ipAddress, port) != WinsockError::Success)
                    break;

                if (udpServer.RecvFrom(dataPacket, MAX_PACKET_SIZE, recvByteCount, ipAddress, port) != WinsockError::Success)
                    break;

                auto parsedPacket = ParsePacket(dataPacket);
                if (std::holds_alternative<DataPacketStc>(parsedPacket) == false)
                    break;

                DataPacketStc& parsedDataPacket = std::get<DataPacketStc>(parsedPacket);
                if (parsedDataPacket.Block == block + 1)
                {
                    block++;
                    if (UtilityLib::FileIO::WriteToFile(file, parsedDataPacket.Data) == false)
                    {
                        break;
                    }
                }
            }
        }

        std::vector<std::string> TftpServerCls::PrepareFileContent(const std::string& fullpath, Mode mode)
        {
            std::string fileContent;
            std::vector<std::string> dataPacketList;

            if (mode == Mode::Octet)
            {
                fileContent = UtilityLib::FileIO::ReadFromFile(fullpath);
            }
            else if (mode == Mode::NetAscii)
            {
                fileContent = UtilityLib::FileIO::ReadFromFile(fullpath);
            }
            else
            {
                return dataPacketList;
            }

            dataPacketList = UtilityLib::String::DivideByLength(fileContent, MAX_DATA_SIZE);
            if (dataPacketList.at(dataPacketList.size() - 1).size() == MAX_DATA_SIZE)
            {
                dataPacketList.push_back(std::string());
            }

            return dataPacketList;
        }
    }
}