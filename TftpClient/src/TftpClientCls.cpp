#include "TftpClientCls.h"
#include <WinSock2.h>

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
	void TftpClientCls::SetIpAddress(const std::string& ipAddress)
	{
		Socket->SetIpAddress(ipAddress);
	}
	
	int TftpClientCls::GetLastWinsockError()
	{
		return Socket->GetLastWsaError();
	}

	TftpErrorCodes TftpClientCls::ReadFile(const std::string& filename, const std::string& pathToSaveFile, Mode mode)
	{
		std::string readRequestPacket = CreateRrqPacket(filename, mode);

		size_t sentBytes = 0;
		ErrorEnum result = Socket->SendTo(readRequestPacket.c_str(), readRequestPacket.size(), sentBytes);
		if (result != ErrorEnum::Success)
		{
			return TftpErrorCodes::WinsockError;
		}

		std::string dataPacket;
		std::string ackPacket;
		std::string fileContent;
		size_t recvBytes = 0;
		uint16_t prevBlock = 0;

		do
		{
			result = Socket->RecvFrom(dataPacket, MAX_PACKET_SIZE);
			if (result != ErrorEnum::Success)
			{
				return TftpErrorCodes::WinsockError;
			}

			recvBytes = dataPacket.size();

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
				result = Socket->SendTo(ackPacket.c_str(), ackPacket.size(), sentBytes);
				if (result != ErrorEnum::Success)
				{
					return TftpErrorCodes::WinsockError;
				}
			}
			else if (std::holds_alternative<Tftp::ErrorPacketStc>(parsedDataPacket) == true)
			{
				Tftp::ErrorPacketStc error = std::get<Tftp::ErrorPacketStc>(parsedDataPacket);

				return error.ErrorCode;
			}
			
		} while (recvBytes == MAX_PACKET_SIZE);

		bool isFileWritten = UtilityLib::FileIO::WriteToFile(pathToSaveFile, fileContent);
		if (isFileWritten)
		{
			return TftpErrorCodes::Success;
		}
		else
		{
			return TftpErrorCodes::CannotSaveReadFileToDisk;
		}
	}

	std::string TftpClientCls::CreateRrqPacket(const std::string& filename, Mode mode)
	{
		const size_t opcodeSize = 2;
		const size_t nullTerminatorSize = 2;

		std::string modeName = MODE_MAPPING.at(mode);
		size_t filenameSize = filename.size();
		size_t modeSize = modeName.size();

		size_t packetSize = opcodeSize + filenameSize + modeSize + nullTerminatorSize;

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
		packet[opcodeSize + filenameSize] = '\0';

		// Mode
		for (size_t i = 0; i < modeSize; i++)
		{
			packet[opcodeSize + filenameSize + 1 + i] = modeName[i];
		}

		// Null terminator
		packet[packetSize - 1] = '\0';

		return packet;
	}
	std::string TftpClientCls::CreateAckPacket(uint16_t block)
	{
		std::string ackPacket(4, '\0');
		ackPacket[1] = static_cast<char>(Opcode::Acknowledgment);
		uint16_t networkOrderBlock = htons(block);

		ackPacket[2] = static_cast<char>(networkOrderBlock & 0xFF);
		ackPacket[3] = static_cast<char>((networkOrderBlock >> 8) & 0xFF);

		return ackPacket;
	}
	std::string TftpClientCls::CreateDataPacket(uint16_t block, const std::string& data)
	{
		return std::string();
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