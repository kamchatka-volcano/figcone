#ifndef FIGCONE_STRINGCONVERTER_H
#define FIGCONE_STRINGCONVERTER_H

#include <figcone_tree/errors.h>
#include <figcone_tree/stringconverter.h>
#include <string>
#include <variant>

namespace figcone::detail {

struct StringConversionError {
    std::string message;
};

template<typename T>
std::variant<T, StringConversionError> convertFromString(const std::string& data)
{
    try {
        auto result = StringConverter<T>::fromString(data);
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
