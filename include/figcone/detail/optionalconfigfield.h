#pragma once
#include <utility>
#include <stdexcept>
#include <optional>

namespace figcone{
class Config;
}

namespace figcone::detail{

template<typename T>
struct OptionalConfigField
{
    static_assert(std::is_base_of_v<Config, T>, "T must be a subclass of ConfigReaderStorage");
    using value_type = T;

    template<class... Args>
    void emplace(Args&& ...args)
    {
        _detail_value_ = T{std::forward<Args>(args)...};
        _detail_hasValue_ = true;
    }

    operator bool() const
    {
        return _detail_hasValue_;
    }

    T* operator->()
    {
        if (!_detail_hasValue_)
            throw std::runtime_error{"bad optional field access"};
        return &_detail_value_;
    }

    const T* operator->() const
    {
        if (!_detail_hasValue_)
            throw std::runtime_error{"bad optional field access"};
        return &_detail_value_;
    }

    T& operator*()
    {
        if (!_detail_hasValue_)
            throw std::runtime_error{"bad optional field access"};
        return _detail_value_;
    }

    const T& operator*() const
    {
        if (!_detail_hasValue_)
            throw std::runtime_error{"bad optional field access"};
        return _detail_value_;
    }

    T _detail_value_;
    bool _detail_hasValue_ = false;
};

}