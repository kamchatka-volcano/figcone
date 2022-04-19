#pragma once
#include "ivalidator.h"
#include "iconfigentity.h"
#include <figcone/errors.h>
#include <functional>

namespace figcone::detail {

template<typename T>
class Validator : public IValidator
{
public:
    Validator(IConfigEntity& entity, T& entityValue, std::function<void(const T&)> validatingFunc)
    : entity_(entity)
    , entityValue_(entityValue)
    , validatingFunc_(std::move(validatingFunc))
    {}

private:
    void validate() override
    {
        try{
            validatingFunc_(entityValue_);
        }
        catch(const ValidationError& e){
            throw ConfigError{entity_.description() + ": " + e.what(), entity_.position()};
        }
    }

    IConfigEntity& entity_;
    T& entityValue_;
    std::function<void(const T&)> validatingFunc_;
};

}