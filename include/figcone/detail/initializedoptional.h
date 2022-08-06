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

    template<typename... TArgs>
    InitializedOptional(TArgs&&... args)
        : value_{std::forward<TArgs>(args)...}
    {
    }

    template<class... Args>
    void emplace(Args&& ...args)
    {
        value_ = T{std::forward<Args>(args)...};
        hasValue_ = true;
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

    InitializedOptional<T>& operator=(std::nullopt_t)
    {
        hasValue_ = false;
        return *this;
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