#pragma once
#include <utility>
#include <stdexcept>
#include <optional>


namespace figcone::detail{

template<typename T>
class InitializedOptional
{
public:
    using value_type = T;

    InitializedOptional()   = default;
    InitializedOptional(std::nullopt_t){};
    template<typename... TArgs, std::enable_if_t<(sizeof...(TArgs)>1)>* = nullptr>
    InitializedOptional(TArgs&&... args)
        : value_{std::forward<TArgs>(args)...}
    {
    }
    template<typename TArg, std::enable_if_t<!std::is_base_of_v<InitializedOptional<T>, std::remove_reference_t<TArg>>>* = nullptr>
    InitializedOptional(TArg&& arg)
        : value_{std::forward<TArg>(arg)}
    {
    }

    template<class... Args>
    void emplace(Args&& ...args)
    {
        value_ = T{std::forward<Args>(args)...};
        hasValue_ = true;
    }

    template<typename TArg, std::enable_if_t<!std::is_base_of_v<InitializedOptional<T>, std::remove_reference_t<TArg>> &&
                                             !std::is_same_v<std::nullopt_t, std::remove_cv_t<std::remove_reference_t<TArg>>>>* = nullptr>
    InitializedOptional& operator=(TArg&& arg)
    {
        value_ = std::forward<TArg>(arg);
        hasValue_ = true;
        return *this;
    }

    void reset()
    {
        hasValue_ = false;
    }

    bool has_value() const
    {
        return hasValue_;
    }

    operator bool() const
    {
        return hasValue_;
    }

    T* operator->()
    {
        if (!hasValue_)
            throw std::bad_optional_access{};
        return &value_;
    }

    const T* operator->() const
    {
        if (!hasValue_)
            throw std::bad_optional_access{};
        return &value_;
    }

    T& operator*()
    {
        if (!hasValue_)
            throw std::bad_optional_access{};
        return value_;
    }

    const T& operator*() const
    {
        if (!hasValue_)
            throw std::bad_optional_access{};
        return value_;
    }

    T& value()
    {
        if (!hasValue_)
            throw std::bad_optional_access{};
        return value_;
    }

    const T& value() const
    {
        if (!hasValue_)
            throw std::bad_optional_access{};
        return value_;
    }

    template<typename U>
    T value_or(U&& defaultValue) const
    {
        return hasValue_ ? value_ : static_cast<T>(std::forward<U>(defaultValue));
    }


private:
    T value_;
    bool hasValue_ = false;
};

}