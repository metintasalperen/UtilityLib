#include "FilePkg.h"

namespace UtilityLib
{
    namespace FileIO
    {
        std::string UtilityLib::FileIO::CreateFullPath(const std::string& filename, const std::string& directoryPath)
        {
            std::string fullPath = directoryPath;

            if (UtilityLib::String::IsEndWith(fullPath, "\\") == false)
            {
                fullPath += "\\";
            }

            fullPath += filename;

            return fullPath;
        }

        bool IsFileExist(const std::string& filePath)
        {
            std::ifstream file(filePath);
            return file.good();
        }

        bool IsFileExist(const std::string& filename, const std::string& directoryPath)
        {
            return IsFileExist(CreateFullPath(filename, directoryPath));
        }
        
        std::string ReadFromFile(const std::string& filePath)
        {
            // Open at the end to get file size
            std::ifstream file(filePath, static_cast<int>(FileMode::ReadBinaryAtTheEnd));

            // File not opened, return empty string
            if (!file)
            {
                return "";
            }

            // Get file size
            std::streamsize size = file.tellg();
            // Move back to the beginning of file
            file.seekg(0);

            // Allocate space to string according to size
            std::string fileContent(static_cast<size_t>(size), '\0');
            // Read file into the string
            file.read(&fileContent[0], size);
            file.close();

            return fileContent;
        }
        
        bool WriteToTextFile(const std::string& filePath, const std::string& content)
        {
            bool result = false;

            std::ofstream file(filePath, static_cast<int>(FileMode::WriteText));
            if (file.is_open())
            {
                file << content;
                file.close();
                result = true;
            }

            return result;
        }

        bool WriteToBinaryFile(const std::string& filePath, const std::string& content)
        {
            bool result = false;

            std::ofstream file(filePath, static_cast<int>(FileMode::WriteBinary));
            if (file.is_open())
            {
                file << content;
                file.close();
                result = true;
            }

            return result;
        }

        bool AppendToTextFile(const std::string& filePath, const std::string& content)
        {
            bool result = false;
            std::ofstream file(filePath, static_cast<int>(FileMode::WriteTextAppend));
            if (file.is_open())
            {
                file << content;
                file.close();
                result = true;
            }
            return result;
        }

        bool AppendToBinaryFile(const std::string& filePath, const std::string& content)
        {
            bool result = false;
            std::ofstream file(filePath, static_cast<int>(FileMode::WriteBinaryAppend));
            if (file.is_open())
            {
                file << content;
                file.close();
                result = true;
            }
            return result;
        }

        std::ofstream OpenFile(const std::string& filePath, FileMode mode)
        {
            return std::ofstream(filePath, static_cast<int>(mode));
        }

        bool WriteToFile(std::ofstream& fileStream, const std::string& content)
        {
            if (fileStream.is_open())
            {
                fileStream << content;
                return true;
            }
            return false;
        }
    };
};
