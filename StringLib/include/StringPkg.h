#ifndef STRINGPKG_H
#define STRINGPKG_H

#include <cstdint>
#include <string>
#include <vector>
#include <cctype>
#include <charconv>
#include <concepts>

namespace UtilityLib
{
    namespace String
    {
        enum class StringError
        {
            Success = 0,
            InvalidArgument,
            OutOfRange
        };

        // Divide()
        // 
        // Summary:
        // Divides a string into pieces according to the specified character
        // 
        // Arguments:
        // std::string "str"  ---  In
        // char "ch"          ---  In
        // 
        // Returns:
        // std::vector<std::string>
        // 
        // Assumptions:
        // If searched character is found twice without any other character in between
        // Empty string will not be added to the list
        // For example; Divide("string1\n\nstring2", '\n') will return ["string1", "string2"] not ["string1", "", "string2"]
        std::vector<std::string> Divide(const std::string& str, const char ch);
        // Divide()
        // 
        // Summary:
        // Divides a string into pieces according to the specified substring
        // 
        // Arguments:
        // std::string "str"    --- In
        // std::string "substr" --- In
        // 
        // Returns:
        // std::vector<std::string>
        // 
        // Assumptions:
        // If searched substring is found twice without any other character in between
        // Empty string will not be added to the list
        // For example; Divide("string1\n\n\n\nstring2", "\n\n") will return ["string1", "string2"] not ["string1", "", "string2"]
        std::vector<std::string> Divide(const std::string& str, const std::string& substr);
        // Filter()
        // 
        // Summary
        // Filters strings that contains the keyword provided, returns the remaining strings as a vector
        // 
        // Arguments:
        // std::vector<std::string> "strList"  --- In
        // std::string "keyword"               --- In
        // 
        // Returns:
        // std::vector<std::string>
        std::vector<std::string> Filter(const std::vector<std::string>& strList, const std::string& keyword);
        // Filter()
        // 
        // Summary
        // Filters strings that contains any of the keywords provided, returns the remaining strings as a vector
        // 
        // Arguments:
        // std::vector<std::string> "strList"  --- In
        // std::vector<std::string> "keyword"  --- In
        // 
        // Returns:
        // std::vector<std::string>
        std::vector<std::string> Filter(const std::vector<std::string>& strList, const std::vector<std::string>& keywordList);
        // LeftTrim()
        // 
        // Summary
        // Removes leading whitespaces from string
        // 
        // Arguments:
        // std::string "str"  --- In
        // 
        // Returns:
        // std::string
        std::string LeftTrim(const std::string& str);
        // RightTrim()
        // 
        // Summary
        // Removes trailing whitespaces from string
        // 
        // Arguments:
        // std::string "str"  --- In
        // 
        // Returns:
        // std::string
        std::string RightTrim(const std::string& str);
        // Trim()
        // 
        // Summary
        // Removes leading and trailing whitespaces from string
        // 
        // Arguments:
        // std::string "str"  --- In
        // 
        // Returns:
        // std::string
        std::string Trim(const std::string& str);
        // Join()
        // 
        // Summary
        // Joins all of the strings in vector into a single string, using a specified character
        // 
        // Arguments:
        // std::vector<std::string> "stringList"  --- In
        // char "ch"                              --- In
        // 
        // Returns:
        // std::string
        std::string Join(const std::vector<std::string>& stringList, const char ch);
        // Join()
        // 
        // Summary
        // Joins all of the strings in vector into a single string, using a specified substring
        // 
        // Arguments:
        // std::vector<std::string> "stringList"  --- In
        // std::string "delimiter"                --- In
        // 
        // Returns:
        // std::string
        std::string Join(const std::vector<std::string>& stringList, const std::string& delimiter);
        // IsStartWith()
        // 
        // Summary
        // Checks if a string starts with specified prefix
        // 
        // Arguments:
        // std::string "str"     --- In
        // std::string "prefix"  --- In
        // 
        // Returns:
        // bool
        bool IsStartWith(const std::string& str, const std::string& prefix);
        // IsEndWith()
        // 
        // Summary
        // Checks if a string ends with specified suffix
        // 
        // Arguments:
        // std::string "str"     --- In
        // std::string "suffix"  --- In
        // 
        // Returns:
        // bool
        // 
        // Assumptions:
        // Do not provide a suffix that is longer than the string itself
        // Result will be undefined
        bool IsEndWith(const std::string& str, const std::string& suffix);
        // Replace()
        // 
        // Summary
        // Replaces first occurence of a substring with another substring
        // 
        // Arguments:
        // std::string "str"        --- In
        // std::string "srcSubstr"  --- In
        // std::string "dstSubstr"  --- In
        // 
        // Returns:
        // std::string
        // 
        // Assumptions:
        // Do not provide "srcSubstr" that is longer than the "str"
        // Result will be undefined
        std::string Replace(const std::string& str, const std::string& srcSubstr, const std::string& dstSubstr);
        // ReplaceAll()
        // 
        // Summary
        // Replaces all occurences of a substring with another substring
        // 
        // Arguments:
        // std::string "str"        --- In
        // std::string "srcSubstr"  --- In
        // std::string "dstSubstr"  --- In
        // 
        // Returns:
        // std::string
        // 
        // Assumptions:
        // Do not provide "srcSubstr" that is longer than the "str"
        // Result will be undefined
        std::string ReplaceAll(const std::string& str, const std::string& srcSubstr, const std::string& dstSubstr);
        // ToLower()
        // 
        // Summary
        // Converts all characters to the lowercase
        // 
        // Arguments:
        // std::string "str"  --- In
        // 
        // Returns:
        // std::string
        std::string ToLower(const std::string& str);
        // ToUpper()
        // 
        // Summary
        // Converts all characters to the uppercase
        // 
        // Arguments:
        // std::string "str"  --- In
        // 
        // Returns:
        // std::string
        std::string ToUpper(const std::string& str);
        // RemoveSubstring()
        // 
        // Summary
        // Removes all occurences of substring from string
        // 
        // Arguments:
        // std::string "str"     --- In
        // std::string "substr"  --- In
        // 
        // Returns:
        // std::string
        // 
        // Assumptions:
        // Do not provide a "substr" that is longer than "str"
        // Result will be undefined
        std::string RemoveSubstring(const std::string& str, const std::string& substr);
        // RemoveSubstrings()
        // 
        // Summary
        // Removes all occurences of all of the substrings from string
        // 
        // Arguments:
        // std::string "str"                      --- In
        // std::vector<std::string> "substrList"  --- In
        // 
        // Returns:
        // std::string
        // 
        // Assumptions:
        // Do not provide a "substr" that is longer than "str"
        // Result will be undefined
        std::string RemoveSubstrings(const std::string& str, const std::vector<std::string>& substrList);
        // RemoveDuplicateChars()
        // 
        // Summary
        // Removes duplicate characters from string
        // 
        // Arguments:
        // std::string "str"  --- In
        // 
        // Returns:
        // std::string
        std::string RemoveDuplicateChars(const std::string& str);
        // DivideToWords()
        // 
        // Summary
        // Divide strings into words vector
        // 
        // Arguments:
        // std::string "str"  --- In
        // 
        // Returns:
        // std::vector<std::string>
        std::vector<std::string> DivideToWords(const std::string& str);
        // RemoveDuplicateWords()
        // 
        // Summary
        // Removes duplicate words from string
        // 
        // Arguments:
        // std::string "str"  --- In
        // 
        // Returns:
        // std::string
        std::string RemoveDuplicateWords(const std::string& str);
        // DivideByLength()
        // 
        // Summary
        // Divides string into pieces with given length
        // Last piece can be smaller than given length
        // 
        // Arguments:
        // const std::string& "str"  --- In
        // size_t partLen            --- In
        // 
        // Returns:
        // std::vector<std::string>
        std::vector<std::string> DivideByLength(const std::string& str, size_t partLen);
        // EncodeBase64()
        // 
        // Summary
        // Encodes provided string to base 64
        // 
        // Arguments:
        // std::string "in"  --- In
        // 
        // Returns:
        // 
        // std::string
        std::string EncodeBase64(const std::string& in);
        // StringToIntegral()
        // 
        // Summary:
        // Transforms string literal to the integral type
        // 
        // Arguments:
        // const std::string& str  --- In
        // T& value                --- Out (T must be integral type)
        // size_t offset           --- In (when no offset is provided, check will start at the start of string)
        // size_t count            --- In (when no count is provided, check will continue until the end of string)
        // int32_t base            --- In (default is 10)
        // 
        // Returns:
        // UtilityLib::Error::ErrorEnum
        template<std::integral T>
        StringError StringToIntegral(const std::string& str, T& value, size_t offset = 0, size_t count = 0, int32_t base = 10)
        {
            if (offset + count > str.size())
            {
                return StringError::InvalidArgument;
            }

            if (count == 0)
            {
                count = str.size() - offset;
            }

            StringError result = StringError::Success;

            const char* start = str.data() + offset;
            const char* end = str.data() + offset + count;

            std::from_chars_result conversionResult = std::from_chars(start, end, value, base);

            if (conversionResult.ec == std::errc::invalid_argument)
            {
                result = StringError::InvalidArgument;
            }
            else if (conversionResult.ec == std::errc::result_out_of_range)
            {
                result = StringError::OutOfRange;
            }

            return result;
        }
        // IsIntegral()
        // 
        // Summary:
        // Checks if the provided string is an integral number
        //
        // Arguments:
        // const std::string& str  --- In
        //
        // Returns:
        // bool
        bool IsIntegral(const std::string& str);
        // IntegralToString()
        // 
        // Summary:
        // Transforms integral type to std::string
        // 
        // Arguments:
        // T val  --- In (T must be integral type)
        // 
        // Returns:
        // std::string
        template<std::integral T>
        std::string IntegralToString(T val)
        {
            std::string result;

            while (val > 0)
            {
                T remainder = val % 10;
                char digitAsChar = static_cast<char>(remainder + '0');
                result.insert(result.begin(), digitAsChar);
                val /= 10;
            }

            return result;
        }
        // ValidateIpAddress()
        // 
        // Summary:
        // Validates the provided string is a valid IP address
        // 
        // Arguments:
        // const std::string& ipAddress  --- In
        // 
        // Returns:
        // bool
        bool ValidateIpAddress(const std::string& ipAddress);
        // ValidatePort()
        // 
        // Summary:
        // Checks if provided string is a valid port number
        // 
        // Arguments:
        // const std::string& port  --- In
        // 
        // Returns:
        // bool
        bool ValidatePort(const std::string& port);
        // Reverse()
        // 
        // Summary:
        // Reverses the string
        // 
        // Arguments:
        // const std::string& str  --- In
        // 
        // Returns:
        // std::string
        std::string Reverse(const std::string& str);
        // ReplaceTabsWithSpaces()
        // 
        // Summary:
        // Replaces \t with spaceCount number of spaces
        // 
        // Arguments:
        // const std::string& str  --- In
        // uint32_t spaceCount     --- In (default = 4)
        // 
        // Returns:
        // std::string
        std::string ReplaceTabsWithSpaces(const std::string& str, uint32_t spaceCount = 4);
    };
};

#endif