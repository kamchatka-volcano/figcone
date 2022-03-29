#pragma once
#include <string_view>
#include <vector>
#include <algorithm>

namespace sfun{
namespace string_utils{

inline std::string trimFront(std::string_view str)
{
    auto it = std::find_if(str.begin(), str.end(), [](int ch) {
        return !std::isspace(ch);
    });
    auto firstNotBlank = std::distance(str.begin(), it);
    return std::string{str.substr(static_cast<std::size_t>(firstNotBlank))};
}

inline std::string trimBack(std::string_view str)
{
    auto it = std::find_if(str.rbegin(), str.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base();
    auto lastNotBlank = std::distance(str.begin(), it);
    return std::string{str.substr(0, static_cast<std::size_t>(lastNotBlank))};
}

inline std::string trim(std::string_view str)
{
    return trimBack(trimFront(str));
}

inline std::vector<std::string> split(std::string_view str, std::string_view delim = " ", bool trimmed = true)
{
    if (delim.empty() || str.empty())
        return std::vector<std::string>{std::string{str}};

    auto result = std::vector<std::string>{};
    auto delimPos = std::size_t{0};
    auto pos = std::size_t{0};
    while (pos < str.size()){
        delimPos = str.find(delim, delimPos);
        if (delimPos == std::string::npos)
            delimPos = str.size();
        auto part = str.substr(pos, delimPos - pos);
        if (trimmed)
            part = trim(part);
        if (!part.empty())
            result.emplace_back(std::string{part});
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

inline std::string before(std::string_view str, std::string_view val)
{
    auto res = str.find(val);
    if (res == std::string::npos)
        return std::string{str};
    return std::string{str.begin(), str.begin() + static_cast<int>(res)};
}

inline std::string after(std::string_view str, std::string_view val)
{
    auto res = str.find(val);
    if (res == std::string::npos)
        return {};
    return std::string{str.begin() + static_cast<int>(res + val.size()), str.end()};
}

inline std::string between(std::string_view str, std::string_view afterStr, std::string_view beforeStr)
{
    return before(after(str, afterStr), beforeStr);
}

}
}

