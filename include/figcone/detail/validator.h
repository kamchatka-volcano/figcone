#ifndef FIGCONE_VALIDATOR_H
#define FIGCONE_VALIDATOR_H

#include "iconfigentity.h"
#include "ivalidator.h"
#include "utils.h"
#include "external/eel/type_traits.h"
#include <figcone/errors.h>
#include <functional>

namespace figcone::detail {

template<typename T>
class Validator : public IValidator {
public:
    Validator(
            IConfigEntity& entity,
            T& entityValue,
            std::function<void(const eel::remove_optional_t<T>&)> validatingFunc)
        : entity_(entity)
        , entityValue_(entityValue)
        , validatingFunc_(std::move(validatingFunc))
    {
    }

private:
    void validate() override
    {
        try {
            if constexpr (eel::is_optional_v<T> || is_initialized_optional_v<T>) {
                if (entityValue_)
                    validatingFunc_(*entityValue_);
            }
            else
                validatingFunc_(entityValue_);
        }
        catch (const ValidationError& e) {
            throw ConfigError{entity_.description() + ": " + e.what(), entity_.position()};
        }
    }

    IConfigEntity& entity_;
    T& entityValue_;
    std::function<void(const eel::remove_optional_t<T>&)> validatingFunc_;
};

} //namespace figcone::detail

#endif //FIGCONE_VALIDATOR_H