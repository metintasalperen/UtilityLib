#include "StringPkg.h"

namespace UtilityLib
{
    namespace String
    {
        std::vector<std::string> Divide(const std::string& str, const char ch)
        {
            std::vector<std::string> strList;
            size_t startIdx = 0;
            size_t stringSize = str.size();

            for (size_t i = 0; i < stringSize; i++)
            {
                if (str[i] == ch)
                {
                    std::string subStr = str.substr(startIdx, i - startIdx);
                    if (subStr.size() != 0)
                    {
                        strList.push_back(subStr);
                        startIdx = i + 1;
                    }
                }
            }

            return strList;
        }
        std::vector<std::string> Divide(const std::string& str, const std::string& substr)
        {
            std::vector<std::string> strList;
            std::string piece;

            // Index of the start of string piece
            size_t startIdx = 0;
            // Index of the first character of substring
            size_t substrFirstChar = str.find(substr);
            // Determined length of string piece
            size_t len = substrFirstChar - startIdx;
            // length of substring
            size_t substrSize = substr.size();

            // Loop through the string until specified substr cannot be found anymore
            while (substrFirstChar != std::string::npos)
            {
                // Add piece to the list
                if (len != 0)
                {
                    piece = str.substr(startIdx, len);
                    strList.push_back(piece);
                }

                // Find the start index of next piece. It is end of previous piece + length of the substring
                startIdx = substrFirstChar + substrSize;

                // Find the start index of  next occurrence of substring
                substrFirstChar = str.find(substr, startIdx);

                // Find the length of substring
                if (substrFirstChar != std::string::npos)
                {
                    len = substrFirstChar - startIdx;
                }
                else
                {
                    len = str.size() - startIdx;
                    if (len != 0)
                    {
                        piece = str.substr(startIdx, len);
                        strList.push_back(piece);
                    }
                }

            }

            return strList;
        }
        std::vector<std::string> Filter(const std::vector<std::string>& strList, const std::string& keyword)
        {
            std::vector<std::string> filteredList;
            size_t size = strList.size();

            for (size_t i = 0; i < size; i++)
            {
                size_t index = strList.at(i).find(keyword);

                // Keyword is not found in string, add the string to the filteredList vector
                if (index == std::string::npos)
                {
                    filteredList.push_back(strList.at(i));
                }
            }

            return filteredList;
        }
        std::vector<std::string> Filter(const std::vector<std::string>& strList, const std::vector<std::string>& keywordList)
        {
            std::vector<std::string> filteredList;
            size_t strListSize = strList.size();
            size_t keywordListSize = keywordList.size();

            for (size_t i = 0; i < strListSize; i++)
            {
                bool keywordFound = false;

                for (size_t j = 0; j < keywordListSize; j++)
                {
                    size_t index = strList.at(i).find(keywordList.at(j));

                    // At least one keyword is found, this string will be filtered
                    if (index != std::string::npos)
                    {
                        keywordFound = true;
                        break;
                    }
                }

                // No keyword is found, at string to the filteredList vector
                if (keywordFound == false)
                {
                    filteredList.push_back(strList.at(i));
                }
            }

            return filteredList;
        }
        std::string LeftTrim(const std::string& str)
        {
            std::string result = str;
            size_t strSize = str.size();

            for (size_t i = 0; i < strSize; i++)
            {
                if (str[i] != ' ')
                {
                    result = str.substr(i, strSize - i);
                    break;
                }
            }

            return result;
        }
        std::string RightTrim(const std::string& str)
        {
            std::string result = str;
            size_t strSize = str.size();

            for (size_t i = strSize - 1; i > 0; i--)
            {
                if (str[i] != ' ')
                {
                    result = str.substr(0, i + 1);
                    break;
                }
            }

            return result;
        }
        std::string Trim(const std::string& str)
        {
            std::string result = LeftTrim(str);
            result = RightTrim(result);
            return result;
        }
        std::string Join(const std::vector<std::string>& stringList, const char ch)
        {
            std::string result;
            size_t listSize = stringList.size();

            for (size_t i = 0; i < listSize; i++)
            {
                result += stringList.at(i);

                // Unless this is the last string, add character in between strings
                if (i != listSize - 1)
                {
                    result += ch;
                }
            }

            return result;
        }
        std::string Join(const std::vector<std::string>& stringList, const std::string& delimiter)
        {
            std::string result;
            size_t listSize = stringList.size();

            for (size_t i = 0; i < listSize; i++)
            {
                result += stringList.at(i);

                // Unless this is the last string, add character in between strings
                if (i != listSize - 1)
                {
                    result += delimiter;
                }
            }

            return result;
        }
        bool IsStartWith(const std::string& str, const std::string& prefix)
        {
            bool result = false;

            size_t index = str.find(prefix);
            if (index == 0)
            {
                result = true;
            }

            return result;
        }
        bool IsEndWith(const std::string& str, const std::string& suffix)
        {
            bool result = false;
            size_t searchStart = str.size() - suffix.size();

            size_t index = str.find(suffix, searchStart);
            if (index == searchStart)
            {
                result = true;
            }

            return result;
        }
        std::string Replace(const std::string& str, const std::string& srcSubstr, const std::string& dstSubstr)
        {
            std::string result = "";
            size_t srcStrSize = srcSubstr.size();

            size_t index = str.find(srcSubstr);

            // Source substring exists
            if (index != std::string::npos)
            {
                // Add string until the source substring
                result = str.substr(0, index);
                // Add destination substring
                result += dstSubstr;
                // Add string after the source substring
                result += str.substr(index + srcStrSize, str.size() - index - srcStrSize);
            }

            return result;
        }
        std::string ReplaceAll(const std::string& str, const std::string& srcSubstr, const std::string& dstSubstr)
        {
            std::string result = "";
            size_t srcStrSize = srcSubstr.size();
            size_t startIndex = 0;
            size_t endIndex = 0;

            do
            {
                endIndex = str.find(srcSubstr, startIndex);
                result += str.substr(startIndex, endIndex - startIndex);
                result += dstSubstr;
                startIndex = endIndex + srcStrSize;

            } while (endIndex != std::string::npos);

            return result;
        }
        std::string ToLower(const std::string& str)
        {
            std::string result = "";
            size_t size = str.size();

            for (size_t i = 0; i < size; i++)
            {
                result += static_cast<const char>(std::tolower(str[i]));
            }

            return result;
        }
        std::string ToUpper(const std::string& str)
        {
            std::string result = "";
            size_t size = str.size();

            for (size_t i = 0; i < size; i++)
            {
                result += static_cast<const char>(std::toupper(str[i]));
            }

            return result;
        }
        std::string RemoveSubstring(const std::string& str, const std::string& substr)
        {
            std::string result = "";
            size_t substrSize = substr.size();
            size_t strSize = str.size();
            size_t startIndex = 0;
            size_t substrStartIndex = str.find(substr);

            if (substrStartIndex == std::string::npos)
            {
                return str;
            }

            while (substrStartIndex != std::string::npos)
            {
                result += str.substr(startIndex, substrStartIndex - startIndex);
                startIndex = substrStartIndex + substrSize;
                substrStartIndex = str.find(substr, startIndex);
            }

            result += str.substr(startIndex, strSize - startIndex);

            return result;
        }
        std::string RemoveSubstrings(const std::string& str, const std::vector<std::string>& substrList)
        {
            std::string result = str;

            for (size_t i = 0; i < substrList.size(); i++)
            {
                result = RemoveSubstring(result, substrList.at(i));
            }

            return result;
        }
        std::string RemoveDuplicateChars(const std::string& str)
        {
            std::string result = "";
            size_t size = str.size();

            for (size_t i = 0; i < size; i++)
            {
                size_t index = result.find(str[i]);

                if (index == std::string::npos)
                {
                    result += str[i];
                }
            }

            return result;
        }
        std::vector<std::string> DivideToWords(const std::string& str)
        {
            std::string word;
            std::vector<std::string> wordList;
            size_t strSize = str.size();
            size_t start = 0;

            for (size_t i = 0; i < strSize; i++)
            {
                bool isLetter = ((str[i] >= 'a') &&
                    (str[i] <= 'z')) ||
                    ((str[i] >= 'A') &&
                        (str[i] <= 'Z'));

                bool isNumber = (str[i] >= '0') &&
                    (str[i] <= '9');

                // If the char is not a letter, number and "'"
                if ((isLetter == false) &&
                    (isNumber == false) &&
                    (str[i] != '\''))
                {
                    word = str.substr(start, i - start);
                    start = i;

                    // Only save the word if it is not a punctiation
                    if (word != "." && word != "," && word != ":" && word != ";" && word != "!" && word != "?")
                    {
                        wordList.push_back(word);
                    }
                }
            }

            // Save the last word if it is not punctiation
            word = str.substr(start, strSize - start);
            if (word != "." && word != "," && word != ":" && word != ";" && word != "!" && word != "?")
            {
                wordList.push_back(word);
            }

            return wordList;
        }
        std::string RemoveDuplicateWords(const std::string& str)
        {
            std::string result = "";
            std::vector<std::string> wordList = DivideToWords(str);

            for (size_t i = 0; i < wordList.size(); i++)
            {
                size_t index = result.find(wordList.at(i));
                if (index == std::string::npos)
                {
                    result += wordList.at(i);
                }
            }

            return result;
        }
        std::vector<std::string> DivideByLength(const std::string& str, size_t partLen)
        {
            std::vector<std::string> parts;
            size_t strLen = str.size();
            size_t startIndex = 0;

            while (strLen > partLen)
            {
                std::string part = str.substr(startIndex, partLen);

                startIndex += partLen;
                strLen -= partLen;

                parts.push_back(part);
            }

            if (strLen > 0)
            {
                parts.push_back(str.substr(startIndex, strLen));
            }

            return parts;

        }
        std::string EncodeBase64(const std::string& in)
        {
            static const std::string BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef\
                                                     ghijklmnopqrstuvwxyz0123456789+/";
            const size_t MASK = 0x3F;

            std::string out;
            int val = 0;
            int valb = -6;

            for (unsigned char c : in)
            {
                val = (val << 8) + c;
                valb += 8;
                while (valb >= 0)
                {
                    out.push_back(BASE64_CHARS[(val >> valb) & MASK]);
                    valb -= 6;
                }
            }

            if (valb > -6)
            {
                out.push_back(BASE64_CHARS[((val << 8) >> (valb + 8)) & MASK]);
            }

            while (out.size() % 4)
            {
                out.push_back('=');
            }

            return out;
        }
    };
};