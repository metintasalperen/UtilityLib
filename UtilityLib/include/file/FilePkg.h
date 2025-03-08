#ifndef FILEPKG_H
#define FILEPKG_H

#include <fstream>
#include <string>

namespace UtilityLib
{
    namespace FileIO
    {
        // IsFileExist()
        // 
        // Summary
        // Check if specified file exist or not
        // 
        // Arguments:
        // std::string "filePath"  --- In
        // 
        // Returns
        // bool
        bool IsFileExist(const std::string& filePath);
        // ReadFile()
        // 
        // Summary
        // Reads the file and returns the content as a single string
        // 
        // Arguments:
        // std::string "filePath"  --- In
        // 
        // Returns:
        // std::string
        std::string ReadFromFile(const std::string& filePath);
        // WriteFile()
        // 
        // Summary
        // Writes string to the specified file
        // 
        // Arguments:
        // std::string "filePath"  --- In
        // std::string "content"   --- In
        // 
        // Returns:
        // bool
        // 
        // Assumptions:
        // 1. If file does not exist, a new file will be created
        // 2. If file contains data, old data is deleted
        bool WriteToFile(const std::string& filePath, const std::string& content);
    };
};

#endif