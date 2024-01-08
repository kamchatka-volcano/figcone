#ifndef FIGCONE_UTILS_H
#define FIGCONE_UTILS_H

#include "initializedoptional.h"
#include "external/sfun/type_traits.h"
#include <optional>
#include <type_traits>
#include <vector>

namespace figcone::detail {

template<typename T, typename = void>
struct is_initialized_optional : std::false_type {};

template<typename T>
struct is_initialized_optional<figcone::detail::InitializedOptional<T>> : std::true_type {};

template<typename T>
inline constexpr auto is_initialized_optional_v = is_initialized_optional<T>::value;

template<typename T>
auto& maybeOptValue(T& obj)
{
    if constexpr (sfun::is_optional_v<T>)
        return *obj;
    else
        return obj;
}

} //namespace figcone::detail

namespace figcone::sfun {
template<typename T>
struct remove_optional<figcone::detail::InitializedOptional<T>> {
    using type = T;
};
} //namespace figcone::sfun

#endif //FIGCONE_UTILS_H