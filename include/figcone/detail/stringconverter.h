#ifndef FIGCONE_STRINGCONVERTER_H
#define FIGCONE_STRINGCONVERTER_H

#include <figcone/detail/external/eel/type_traits.h>
#include <figcone_tree/errors.h>
#include <figcone_tree/stringconverter.h>
#include <string>
#include <variant>

namespace figcone::detail {

struct StringConversionError {
    std::string message;
};

template<typename T>
std::optional<T> fromString(const std::string& data)
{
    [[maybe_unused]] auto setValue = [](auto& value, const std::string& data) -> std::optional<T>
    {
        auto stream = std::stringstream{data};
        stream >> value;

        if (stream.bad() || stream.fail() || !stream.eof())
            return {};
        return value;
    };

    if constexpr (std::is_convertible_v<std::string, tree::eel::remove_optional_t<T>>) {
        return data;
    }
    else if constexpr (tree::eel::is_optional_v<T>) {
        auto value = T{};
        value.emplace();
        return setValue(*value, data);
    }
    else {
        auto value = T{};
        return setValue(value, data);
    }
}

template<typename T>
std::variant<T, StringConversionError> convertFromString(const std::string& data)
{
    try {
        const auto result = [&]
        {
            if constexpr (eel::is_complete_type_v<StringConverter<T>>)
                return StringConverter<T>::fromString(data);
            else
                return fromString<T>(data);
        }();

        if (!result.has_value())
            return StringConversionError{};
        else
            return result.value();
    }
    catch (const ValidationError& error) {
        return StringConversionError{error.what()};
    }
    catch (...) {
        return StringConversionError{};
    }
}

} //namespace figcone::detail

#endif //FIGCONE_STRINGCONVERTER_H
