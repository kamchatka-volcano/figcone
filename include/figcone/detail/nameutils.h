#pragma once
#include "string_utils.h"
#include <string>
#include <algorithm>

namespace figcone::detail{
namespace str = string_utils;

inline std::string formatName(const std::string& name)
{
    auto result = name;
    //remove front non-alphabet characters
    result.erase(result.begin(), std::find_if(result.begin(), result.end(),
                                              [](int ch){
                                                  return std::isalpha(ch);
                                              })
    );
    //remove back non-alphabet and non-digit characters
    result.erase(std::find_if(result.rbegin(), result.rend(),
                              [](int ch){ return std::isalnum(ch);}).base(), result.end());
    return result;
}

inline std::string toCamelCase(const std::string& name)
{
    auto result = std::string{};
    auto prevCharNonAlpha = false;
    auto formattedName = formatName(name);
    if (!formattedName.empty())
        formattedName[0] = static_cast<char>(std::tolower(formattedName[0]));
    for (auto ch : formattedName){
        if (!std::isalpha(ch)){
            if (std::isdigit(ch))
                result.push_back(ch);
            if (!result.empty())
                prevCharNonAlpha = true;
            continue;
        }
        if (prevCharNonAlpha)
            ch = static_cast<char>(std::toupper(ch));
        result.push_back(ch);
        prevCharNonAlpha = false;
    }
    return result;
}

inline std::string toKebabCase(const std::string& name)
{
    auto result = std::string{};
    auto formattedName = formatName(str::replace(name, "_", "-"));
    if (!formattedName.empty())
        formattedName[0] = static_cast<char>(std::tolower(formattedName[0]));
    for (auto ch : formattedName){
        if (std::isupper(ch) && !result.empty()){
            result.push_back('-');
            result.push_back(static_cast<char>(std::tolower(ch)));
        }
        else
            result.push_back(ch);
    }
    return result;
}

inline std::string toSnakeCase(const std::string& name)
{
    auto result = std::string{};
    auto formattedName = formatName(name);
    if (!formattedName.empty())
        formattedName[0] = static_cast<char>(std::tolower(formattedName[0]));
    for (auto ch : formattedName){
        if (std::isupper(ch) && !result.empty()){
            result.push_back('_');
            result.push_back(static_cast<char>(std::tolower(ch)));
        }
        else
            result.push_back(ch);
    }
    return result;
}

}
