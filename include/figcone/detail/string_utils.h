#pragma once
#include <string_view>
#include <vector>
#include <algorithm>
#include <cctype>


namespace figcone::detail::string_utils{

inline bool isalnum(char ch)
{
    return std::isalnum(static_cast<unsigned char>(ch));
}

inline bool isalpha(char ch)
{
    return std::isalpha(static_cast<unsigned char>(ch));
}

inline bool isblank(char ch)
{
    return std::isblank(static_cast<unsigned char>(ch));
}

inline bool iscntrl(char ch)
{
    return std::iscntrl(static_cast<unsigned char>(ch));
}
inline bool isdigit(char ch)
{
    return std::isdigit(static_cast<unsigned char>(ch));
}

inline bool isgraph(char ch)
{
    return std::isgraph(static_cast<unsigned char>(ch));
}

inline bool islower(char ch)
{
    return std::islower(static_cast<unsigned char>(ch));
}

inline bool isprint(char ch)
{
    return std::isprint(static_cast<unsigned char>(ch));
}

inline bool ispunct(char ch)
{
    return std::ispunct(static_cast<unsigned char>(ch));
}

inline bool isspace(char ch)
{
    return std::isspace(static_cast<unsigned char>(ch));
}

inline bool isupper(char ch)
{
    return std::isupper(static_cast<unsigned char>(ch));
}

inline bool isxdigit(char ch)
{
    return std::isxdigit(static_cast<unsigned char>(ch));
}

inline char tolower(char ch)
{
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

inline char toupper(char ch)
{
    return static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
}

inline std::string_view trimFront(std::string_view str)
{
    auto it = std::find_if(str.begin(), str.end(), [](char ch){
        return !string_utils::isspace(ch);
    });
    auto firstNotBlank = std::distance(str.begin(), it);
    return str.substr(static_cast<std::size_t>(firstNotBlank));
}

inline std::string_view trimBack(std::string_view str)
{
    auto it = std::find_if(str.rbegin(), str.rend(), [](char ch){
        return !string_utils::isspace(ch);
    }).base();
    auto lastNotBlank = std::distance(str.begin(), it);
    return str.substr(0, static_cast<std::size_t>(lastNotBlank));
}

inline std::string_view trim(std::string_view str)
{
    return trimBack(trimFront(str));
}

inline std::vector<std::string_view> split(std::string_view str, std::string_view delim = " ", bool trimmed = true)
{
    if (delim.empty() || str.empty())
        return std::vector<std::string_view>{str};

    auto result = std::vector<std::string_view>{};
    auto delimPos = std::size_t{0};
    auto pos = std::size_t{0};
    while (pos < str.size()){
        delimPos = str.find(delim, delimPos);
        if (delimPos == std::string_view::npos)
            delimPos = str.size();
        auto part = str.substr(pos, delimPos - pos);
        if (trimmed)
            part = trim(part);
        if (!part.empty())
            result.emplace_back(part);
        delimPos += delim.size();
        pos = delimPos;
    }
    return result;
}

inline std::string replace(std::string str, std::string_view subStr, std::string_view val)
{
    if (subStr.empty())
        return str;

    auto pos = str.find(subStr);
    while (pos != std::string::npos){
        str.replace(pos, subStr.size(), val);
        pos = str.find(subStr, pos + val.size());
    }
    return str;
}

inline bool startsWith(std::string_view str, std::string_view val)
{
    auto res = str.find(val);
    return res == 0;
}

inline bool endsWith(std::string_view str, std::string_view val)
{
    if (val.empty())
        return true;
    auto res = std::find_end(str.begin(), str.end(), val.begin(), val.end());
    if (res == str.end())
        return false;
    return std::distance(res, str.end()) == static_cast<int>(val.size());
}

inline std::string_view before(std::string_view str, std::string_view val)
{
    auto res = str.find(val);
    if (res == std::string_view::npos)
        return str;
    return {str.begin(), res};
}

inline std::string_view after(std::string_view str, std::string_view val)
{
    auto res = str.find(val);
    if (res == std::string_view::npos)
        return {};
    return {str.begin() + static_cast<int>(res + val.size()), str.size() - (res + val.size())};
}

inline std::string_view between(std::string_view str, std::string_view afterStr, std::string_view beforeStr)
{
    return before(after(str, afterStr), beforeStr);
}

}

