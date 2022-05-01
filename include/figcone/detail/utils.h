#pragma once
#include <optional>
#include <vector>
#include <type_traits>

namespace figcone::detail {

template<typename T, typename = void>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<std::optional<T> > : std::true_type {};


template <typename, typename = void>
struct is_sequence_container : std::false_type{};

template <typename T>
struct is_sequence_container<T,
    std::void_t< typename T::value_type,
                 decltype(std::declval<T>().begin()),
                 decltype(std::declval<T>().end()),
                 decltype(std::declval<T>().emplace_back(std::declval<typename T::value_type>()))>
> : std::true_type{};

template <typename T>
inline constexpr auto is_sequence_container_v = is_sequence_container<T>::value;

template<typename T>
auto& maybeOptValue(T& obj)
{
    if constexpr(is_optional<T>::value)
        return *obj;
    else
        return obj;
}

template<typename T, typename = void>
struct maybe_opt{
    using type = T;
};
template<typename T>
struct maybe_opt<std::optional<T>>{
    using type = T;
};

template<typename T>
using maybe_opt_t = typename maybe_opt<T>::type;


}