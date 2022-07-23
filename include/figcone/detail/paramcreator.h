#pragma once
#include "iconfigreader.h"
#include "param.h"
#include "validator.h"
#include "gsl_assert.h"
#include "figcone/config.h"

namespace figcone::detail{

template<typename T>
class ParamCreator{
public:
    ParamCreator(ConfigReaderPtr cfg,
                 std::string paramName,
                 T& paramValue)
        : cfg_{cfg}
        , paramName_{(Expects(!paramName.empty()), std::move(paramName))}
        , paramValue_{paramValue}
        , param_{std::make_unique<Param<T>>(paramName_, paramValue)}
    {
    }

    ParamCreator<T>& operator()(T defaultValue = {})
    {
        defaultValue_ = std::move(defaultValue);
        param_->markValueIsSet();
        return *this;
    }

    ParamCreator<T>& ensure(std::function<void(const T&)> validatingFunc)
    {
        if (cfg_)
            cfg_->addValidator(std::make_unique<Validator<T>>(*param_, paramValue_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    ParamCreator<T>& ensure(TArgs&&... args)
    {
        if (cfg_)
            cfg_->addValidator(std::make_unique<Validator<T>>(*param_, paramValue_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

    operator T()
    {
        if (cfg_)
            cfg_->addParam(paramName_, std::move(param_));
        return defaultValue_;
    }

private:
    ConfigReaderPtr cfg_;
    std::string paramName_;
    T& paramValue_;
    std::unique_ptr<Param<T>> param_;
    T defaultValue_;
};

template<typename T>
ParamCreator<T> makeParamCreator(ConfigReaderPtr cfgReader,
                                 std::string paramName,
                                 const std::function<T&()>& paramGetter)
{
    return {cfgReader, std::move(paramName), paramGetter()};
}

}