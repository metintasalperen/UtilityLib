#include <iostream>
#include "SocketPkg.h"
#include "TftpClientCls.h"

int main()
{
	std::string ipAddr;
	std::cout << "Ip Address of TFTP Server: ";
	std::getline(std::cin, ipAddr);
	
	auto tftpClient = Tftp::TftpClientCls::Initialize(ipAddr);
	if (tftpClient)
	{
		std::string fileName;
		std::cout << "File name to read: ";
		std::getline(std::cin, fileName);

		std::string path;
		std::cout << "Where to save: ";
		std::getline(std::cin, path);

		auto result = tftpClient->ReadFile(fileName, path, Tftp::Mode::NetAscii);
		if (result == Tftp::TftpErrorCodes::Success)
		{
			std::cout << "File received and successfully saved to " << path << "\n";
			return 0;
		}
		else
		{
			std::cerr << "Some error occurred while reading... Error code: " << Tftp::ERROR_MESSAGES.at(result) << "\n";
			return -1;
		}
	}
	else
	{
		std::cerr << "Cannot open Tftp App, please restart\n";
		return -1;
	}
}