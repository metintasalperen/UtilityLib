#include "FilePkg.h"

namespace UtilityLib
{
    namespace FileIO
    {
        bool IsFileExist(const std::string& filePath)
        {
            std::ifstream file(filePath);
            return file.good();
        }
        std::string ReadFromFile(const std::string& filePath)
        {
            // Open at the end to get file size
            std::ifstream file(filePath, std::ios::ate);

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
        bool WriteToFile(const std::string& filePath, const std::string& content)
        {
            bool result = false;

            std::ofstream file(filePath);
            if (file.is_open())
            {
                file << content;
                file.close();
                result = true;
            }

            return result;
        }
    };
};
