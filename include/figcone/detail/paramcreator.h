#pragma once
#include "iconfig.h"
#include "param.h"
#include "validator.h"
#include "gsl_assert.h"

namespace figcone::detail{

template<typename T>
class ParamCreator{
public:
    ParamCreator(IConfig& cfg,
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
        cfg_.addValidator(std::make_unique<Validator<T>>(*param_, paramValue_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    ParamCreator<T>& ensure(TArgs&&... args)
    {
        cfg_.addValidator(std::make_unique<Validator<T>>(*param_, paramValue_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

    operator T()
    {
        cfg_.addParam(paramName_, std::move(param_));
        return defaultValue_;
    }

private:
    IConfig& cfg_;
    std::string paramName_;
    T& paramValue_;
    std::unique_ptr<Param<T>> param_;
    T defaultValue_;
};

template<typename T>
ParamCreator<T> makeParamCreator(IConfig& cfg,
                                 std::string paramName,
                                 const std::function<T&()>& paramGetter)
{
    return {cfg, std::move(paramName), paramGetter()};
}

}