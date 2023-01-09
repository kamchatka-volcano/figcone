#pragma once
#include "nameutils.h"
#include <figcone/nameformat.h>

namespace figcone::detail {

template<NameFormat>
struct NameConverter;

template<>
struct NameConverter<NameFormat::Original> {
    static std::string name(const std::string& configName)
    {
        return formatName(configName);
    }
};

template<>
struct NameConverter<NameFormat::SnakeCase> {
    static std::string name(const std::string& configName)
    {
        return toSnakeCase(configName);
    }
};

template<>
struct NameConverter<NameFormat::CamelCase> {
    static std::string name(const std::string& configName)
    {
        return toCamelCase(configName);
    }
};

template<>
struct NameConverter<NameFormat::KebabCase> {
    static std::string name(const std::string& configName)
    {
        return toKebabCase(configName);
    }
};

} //namespace figcone::detail