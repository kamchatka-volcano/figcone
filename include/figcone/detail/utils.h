#pragma once
#include "initializedoptional.h"
#include <optional>
#include <vector>
#include <type_traits>
#include <sfun/traits.h>

namespace figcone::detail {
using namespace sfun::traits;

template<typename T, typename = void>
struct is_initialized_optional : std::false_type {
};

template<typename T>
struct is_initialized_optional<figcone::detail::InitializedOptional<T>> : std::true_type {
};

template<typename T>
inline constexpr auto is_initialized_optional_v = is_initialized_optional<T>::value;

template<typename T>
auto& maybeOptValue(T& obj)
{
    if constexpr (is_optional_v<T>)
        return *obj;
    else
        return obj;
}

}

namespace sfun::traits {
template<typename T>
struct remove_optional<figcone::detail::InitializedOptional<T>> {
    using type = T;
};
}