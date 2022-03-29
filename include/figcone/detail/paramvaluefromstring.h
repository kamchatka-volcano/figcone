#pragma once
#include <string>
#include <filesystem>
#include <sstream>

namespace figcone {
namespace detail {
template<typename T>
bool paramValueFromStringImpl(const std::string& data, T& value)
{
    if constexpr(std::is_assignable_v<T, std::string>){
        value = data;
        return true;
    }
    else {
        auto stream = std::stringstream{data};
        stream >> value;

        if (stream.bad() || stream.fail() || !stream.eof())
            return false;
        return true;
    }
}

}

template <typename T>
bool paramValueFromString(const std::string& data, T& value)
{
    return detail::paramValueFromStringImpl<T>(data, value);
}

}