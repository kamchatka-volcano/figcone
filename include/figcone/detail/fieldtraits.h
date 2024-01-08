#ifndef FIGCONE_FIELDTRAITS_H
#define FIGCONE_FIELDTRAITS_H
#include "external/sfun/type_list.h"
#include <type_traits>

namespace figcone {

template<auto FieldPtr>
struct OptionalField {
    using optionalField = OptionalField<FieldPtr>;
    static constexpr auto fieldPtr = FieldPtr;
};

template<auto FieldPtr>
struct CopyNodeListField {
    using copyNodeListField = CopyNodeListField<FieldPtr>;
    static constexpr auto fieldPtr = FieldPtr;
};

template<auto FieldPtr, typename TValidator>
struct ValidatedField {
    using validator = TValidator;
    static constexpr auto fieldPtr = FieldPtr;
};

template<typename... Ts>
using FieldTraits = sfun::type_list<Ts...>;

namespace detail {

template<auto memberPtr, typename T, typename = void>
struct IsMemberPtr : std::false_type {};

template<auto memberPtr, typename T>
struct IsMemberPtr<memberPtr, T, std::void_t<decltype(std::declval<T>().*memberPtr)>> : std::true_type {};

template<typename T, typename = void>
struct IsOptionalField : std::false_type {};

template<typename T>
struct IsOptionalField<T, std::void_t<typename T::optionalField>> : std::true_type {};

template<typename T>
using IsOptionalFieldTrait = IsOptionalField<T>;

template<typename T, typename = void>
struct IsCopyNodeListField : std::false_type {};

template<typename T>
struct IsCopyNodeListField<T, std::void_t<typename T::copyNodeListField>> : std::true_type {};

template<typename T>
using IsCopyNodeListFieldTrait = IsCopyNodeListField<T>;

template<typename T, typename = void>
struct IsValidatedField : std::false_type {};

template<typename T>
struct IsValidatedField<T, std::void_t<typename T::validator>> : std::true_type {};

template<typename T, typename = void>
struct HasFieldTraits : std::false_type {};

template<typename T>
struct HasFieldTraits<T, std::void_t<typename T::traits>> : std::true_type {};

template<typename T>
constexpr auto hasFieldTraits = HasFieldTraits<T>::value;

template<template<typename> typename TTraitChecker, typename TTrait, typename TCfg, typename TFieldValue>
bool isTraitAndMatchesField(const TTrait&, TCfg& cfg, TFieldValue& field)
{
    if constexpr (!TTraitChecker<typename TTrait::type>::value)
        return false;
    else {
        static_assert(
                IsMemberPtr<typename TTrait::type{}.fieldPtr, TCfg>::value,
                "Field trait is invalid as it contains a pointer to a member of a different config structure.");
        auto& traitField = cfg.*(typename TTrait::type{}.fieldPtr);
        if constexpr (std::is_same_v<decltype(traitField), decltype(field)>)
            return std::addressof(traitField) == std::addressof(field);
        else
            return false;
    }
}

template<
        template<typename>
        typename TTraitChecker,
        typename TTraitList,
        typename TCfg,
        typename TFieldValue,
        std::size_t... indices>
bool hasMatchingTrait(TCfg& cfg, TFieldValue& field, std::index_sequence<indices...>)
{
    return (isTraitAndMatchesField<TTraitChecker>(sfun::get<indices>(TTraitList{}), cfg, field) || ...);
}

template<typename TCfg, typename TFieldValue>
bool isOptionalField(TCfg& cfg, TFieldValue& field)
{
    if constexpr (detail::hasFieldTraits<TCfg>)
        return hasMatchingTrait<IsOptionalFieldTrait, typename TCfg::traits>(
                cfg,
                field,
                std::make_index_sequence<TCfg::traits::size()>());
    else
        return false;
}

template<typename TCfg, typename TFieldValue>
bool isCopyNodeListField(TCfg& cfg, TFieldValue& field)
{
    if constexpr (detail::hasFieldTraits<TCfg>)
        return hasMatchingTrait<IsCopyNodeListFieldTrait, typename TCfg::traits>(
                cfg,
                field,
                std::make_index_sequence<TCfg::traits::size()>());
    else
        return false;
}

template<typename TTrait, typename TCfg, typename TFieldValue, typename TFieldCreator>
void setValidatorIfTraitMatchesField(const TTrait&, TCfg& cfg, TFieldValue& field, TFieldCreator& fieldCreator)
{
    if constexpr (IsValidatedField<typename TTrait::type>::value) {
        static_assert(
                IsMemberPtr<typename TTrait::type{}.fieldPtr, TCfg>::value,
                "Field trait is invalid as it contains a pointer to a member of a different config structure.");
        auto& traitField = cfg.*(typename TTrait::type{}.fieldPtr);
        if constexpr (std::is_same_v<decltype(traitField), decltype(field)>)
            if (std::addressof(traitField) == std::addressof(field))
                fieldCreator.template ensure<typename TTrait::type::validator>();
    }
}

template<typename TTraitList, typename TCfg, typename TFieldValue, typename TFieldCreator, std::size_t... indices>
void setValidatorFromTraits(TCfg& cfg, TFieldValue& field, TFieldCreator& fieldCreator, std::index_sequence<indices...>)
{
    (setValidatorIfTraitMatchesField(sfun::get<indices>(TTraitList{}), cfg, field, fieldCreator), ...);
}

template<typename TCfg, typename TFieldValue, typename TFieldCreator>
void setFieldValidators(TCfg& cfg, TFieldValue& field, TFieldCreator& fieldCreator)
{
    if constexpr (detail::hasFieldTraits<TCfg>)
        setValidatorFromTraits<typename TCfg::traits>(
                cfg,
                field,
                fieldCreator,
                std::make_index_sequence<TCfg::traits::size()>());
}

} //namespace detail
} //namespace figcone

#endif //FIGCONE_FIELDTRAITS_H
