#ifndef FILEPKG_H
#define FILEPKG_H

#include <fstream>
#include <string>

#include "StringPkg.h"

namespace UtilityLib
{
    namespace FileIO
    {
        enum class FileMode
        {
            WriteBinary = std::ios::out | std::ios::binary,
            WriteBinaryAppend = std::ios::out | std::ios::app | std::ios::binary,
            WriteText = std::ios::out,
            WriteTextAppend = std::ios::out | std::ios::app,
            ReadText = std::ios::in,
            ReadTextAtTheEnd = std::ios::in | std::ios::ate,
            ReadBinary = std::ios::in | std::ios::binary,
            ReadBinaryAtTheEnd = std::ios::in | std::ios::binary | std::ios::ate
        };

        // CreateFullPath()
        // 
        // Summary:
        // Creates a full path by combining filename and directory path
        // 
        // Arguments:
        // const std::string& filename       --- In
        // const std::string& directoryPath  --- In
        // 
        // Returns:
        // std::string
        std::string CreateFullPath(const std::string& filename, const std::string& directoryPath);

        // IsFileExist()
        // 
        // Summary:
        // Check if specified file exist or not
        // 
        // Arguments:
        // const std::string& filePath  --- In
        // 
        // Returns
        // bool
        bool IsFileExist(const std::string& filePath);

        // IsFileExist()
        // 
        // Summary:
        // Check if specified file exist or not
        // 
        // Arguments:
        // const std::string& filename       --- In
        // const std::string& directoryPath  --- In
        // 
        // Returns
        // bool
        bool IsFileExist(const std::string& filename, const std::string& directoryPath);

        // ReadFromFile()
        // 
        // Summary:
        // Reads the file and returns the content as a single string
        // 
        // Arguments:
        // const std::string& filePath  --- In
        // 
        // Returns:
        // std::string
        std::string ReadFromFile(const std::string& filePath);

        // WriteToTextFile()
        // 
        // Summary:
        // Writes string to the specified file
        // 
        // Arguments:
        // const std::string& filePath  --- In
        // const std::string& content   --- In
        // 
        // Returns:
        // bool
        // 
        // Assumptions:
        // 1. If file does not exist, a new file will be created
        // 2. If file contains data, old data is deleted
        // 
        // Important: This function will be slow for frequent writes because it will open and close the file every time
        bool WriteToTextFile(const std::string& filePath, const std::string& content);

        // WriteToBinaryFile()
        // 
        // Summary:
        // Writes string to the specified file
        // 
        // Arguments:
        // const std::string& filePath  --- In
        // const std::string& content   --- In
        // 
        // Returns:
        // bool
        // 
        // Assumptions:
        // 1. If file does not exist, a new file will be created
        // 2. If file contains data, old data is deleted
        // 
        // Important: This function will be slow for frequent writes because it will open and close the file every time
        bool WriteToBinaryFile(const std::string& filePath, const std::string& content);

        // AppendToTextFile()
        // 
        // Summary:
        // Appends string to the end of the specified file
        // 
        // Arguments:
        // const std::string& filePath  --- In
        // const std::string& content   --- In
        // 
        // Returns:
        // bool
        // 
        // Assumptions:
        // 1. If file does not exist, a new file will be created
        // 2. If file contains data, old data will be preserved, and new data will be appended
        // 
        // Important: This function will be slow for frequent writes because it will open and close the file every time
        bool AppendToTextFile(const std::string& filePath, const std::string& content);

        // AppendToBinaryFile()
        // 
        // Summary:
        // Appends string to the end of the specified file
        // 
        // Arguments:
        // const std::string& filePath  --- In
        // const std::string& content   --- In
        // 
        // Returns:
        // bool
        // 
        // Assumptions:
        // 1. If file does not exist, a new file will be created
        // 2. If file contains data, old data will be preserved, and new data will be appended
        // 
        // Important: This function will be slow for frequent writes because it will open and close the file every time
        bool AppendToBinaryFile(const std::string& filePath, const std::string& content);

        // OpenFile()
        // 
        // Summary:
        // Opens the file with specified mode
        // 
        // Arguments:
        // const std::string& filePath  --- In
        // FileMode mode                --- In
        // 
        // Returns:
        // std::ofstream
        std::ofstream OpenFile(const std::string& filePath, FileMode mode);

        // WriteToFile()
        // 
        // Summary:
        // Writes string to the specified file
        // 
        // Arguments:
        // std::ofstream& fileStream  --- In
        // const std::string& content --- In
        // 
        // Returns:
        // bool
        // 
        // Important: For frequent writes, use this function to avoid opening and closing the file every time
        // Create a std::ofstream object with flags using FileMode enum, and pass it to this function
        // After writing all the data, close the file using std::ofstream::close()
        bool WriteToFile(std::ofstream& fileStream, const std::string& content);
    };
};

#endif