#ifndef TFTPCLIENTTYPEPKG_H
#define TFTPCLIENTTYPEPKG_H

#include <map>
#include <string>

namespace UtilityLib
{
    namespace Tftp
    {
        enum class Opcode
        {
            Invalid = 0,
            ReadRequest = 1,
            WriteRequest = 2,
            Data = 3,
            Acknowledgment = 4,
            Error = 5
        };
        enum class TftpError
        {
            // TFTP Specification Error Codes
            NotDefined = 0,
            FileNotFound = 1,
            AccessViolation = 2,
            DiskFullAllocationExceeded = 3,
            IllegalTftpOperation = 4,
            UnknownTransferId = 5,
            FileAlreadyExists = 6,
            NoSuchUser = 7,

            // From now on, these errors are not belong to TFTP Specification but internal error states
            UnknownErrorCode, // Server returned an ERROR message but Error Code is something not specified in TFTP Specification
            WinsockError, // Check return value of GetLastWinsockError() to see specific error code
            InvalidIpAddress, // Invalid IP Address
            CannotSaveReadFileToDisk, // If file cannot be written to the specified path after file is read from server
            UndefinedResponse, // Tftp Server returned undefined response
            FileAtSpecifiedPathNotFound, // File at the specified path is not found
            Success
        };
        enum class Mode
        {
            NetAscii = 0,
            Octet = 1,
            Mail = 2,
            Invalid
        };

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
            TftpError ErrorCode;
            std::string ErrorMessage;
        };

        struct RrqWrqPacketStc
        {
            Opcode Opcode;
            std::string Filename;
            Mode Mode;
        };


        const std::string TFTP_PORT = "69";
        const size_t OPCODE_SIZE = 2;
        const size_t BLOCK_SIZE = 2;
        const size_t ACK_PACKET_SIZE = OPCODE_SIZE + BLOCK_SIZE;
        const size_t MIN_PACKET_SIZE = ACK_PACKET_SIZE;
        const size_t MAX_DATA_SIZE = 512;
        const size_t MAX_PACKET_SIZE = OPCODE_SIZE + BLOCK_SIZE + MAX_DATA_SIZE;
        const std::map<TftpError, std::string> ERROR_MESSAGES({ { TftpError::NotDefined, "Unknown request" },
                                                                { TftpError::FileNotFound, "File does not exist" },
                                                                { TftpError::AccessViolation, "Access not permitted" },
                                                                { TftpError::DiskFullAllocationExceeded, "Disk full or allocation exceeded" },
                                                                { TftpError::IllegalTftpOperation, "TFTP Operation is illegal" },
                                                                { TftpError::UnknownTransferId, "Unknown transfer ID" },
                                                                { TftpError::FileAlreadyExists, "File already exists" },
                                                                { TftpError::NoSuchUser, "Specified user does not exist" },
                                                                { TftpError::UnknownErrorCode, "TFTP server returned an unknown error code" },
                                                                { TftpError::WinsockError, "Network not available" },
                                                                { TftpError::CannotSaveReadFileToDisk, "Can not write to the provided path" },
                                                                { TftpError::UndefinedResponse, "TFTP Server returned an unknown message" },
                                                                { TftpError::FileAtSpecifiedPathNotFound, "File does not exist at the specified path" } });
        const std::map<Mode, std::string> MODE_MAPPING({ { Mode::NetAscii, "netascii" },
                                                         { Mode::Octet, "octet" },
                                                         { Mode::Mail, "mail" } });
    }
}

#endif