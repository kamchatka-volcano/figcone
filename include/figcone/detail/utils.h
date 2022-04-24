#pragma once
#include <optional>
#include <type_traits>

namespace figcone::detail {

template<typename T, typename = void>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<std::optional<T> > : std::true_type {};

}