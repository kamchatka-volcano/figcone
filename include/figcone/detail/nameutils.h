#ifndef FIGCONE_NAMEUTILS_H
#define FIGCONE_NAMEUTILS_H

#include "external/sfun/string_utils.h"
#include "external/sfun/utility.h"
#include <figcone/nameformat.h>
#include <algorithm>
#include <string>

namespace figcone::detail {

inline std::string formatName(const std::string& name)
{
    auto result = name;
    //remove front non-alphabet characters
    result.erase(
            result.begin(),
            std::find_if(
                    result.begin(),
                    result.end(),
                    [](auto ch)
                    {
                        return sfun::isalpha(ch);
                    }));
    //remove back non-alphabet and non-digit characters
    result.erase(
            std::find_if(
                    result.rbegin(),
                    result.rend(),
                    [](auto ch)
                    {
                        return sfun::isalnum(ch);
                    })
                    .base(),
            result.end());
    return result;
}

inline std::string toCamelCase(const std::string& name)
{
    auto result = std::string{};
    auto prevCharNonAlpha = false;
    auto formattedName = formatName(name);
    if (!formattedName.empty())
        formattedName[0] = static_cast<char>(sfun::tolower(formattedName[0]));
    for (auto ch : formattedName) {
        if (!sfun::isalpha(ch)) {
            if (sfun::isdigit(ch))
                result.push_back(ch);
            if (!result.empty())
                prevCharNonAlpha = true;
            continue;
        }
        if (prevCharNonAlpha)
            ch = static_cast<char>(sfun::toupper(ch));
        result.push_back(ch);
        prevCharNonAlpha = false;
    }
    return result;
}

inline std::string toKebabCase(const std::string& name)
{
    auto result = std::string{};
    auto formattedName = formatName(sfun::replace(name, "_", "-"));
    if (!formattedName.empty())
        formattedName[0] = sfun::tolower(formattedName[0]);
    for (auto ch : formattedName) {
        if (sfun::isupper(ch) && !result.empty()) {
            result.push_back('-');
            result.push_back(sfun::tolower(ch));
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
        formattedName[0] = sfun::tolower(formattedName[0]);
    for (auto ch : formattedName) {
        if (sfun::isupper(ch) && !result.empty()) {
            result.push_back('_');
            result.push_back(sfun::tolower(ch));
        }
        else
            result.push_back(ch);
    }
    return result;
}

inline std::string convertName(NameFormat nameFormat, const std::string& configName)
{
    switch (nameFormat) {
    case NameFormat::Original:
        return formatName(configName);
    case NameFormat::SnakeCase:
        return toSnakeCase(configName);
    case NameFormat::CamelCase:
        return toCamelCase(configName);
    case NameFormat::KebabCase:
        return toKebabCase(configName);
    }
    sfun::unreachable();
}

} //namespace figcone::detail

#endif //FIGCONE_NAMEUTILS_H