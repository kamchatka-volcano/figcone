#pragma once
#include "iconfigreader.h"
#include "param.h"
#include "validator.h"
#include "external/sfun/contract.h"

namespace figcone::detail{

template<typename T>
class ParamCreator{
public:
    ParamCreator(ConfigReaderPtr cfgReader,
                 std::string paramName,
                 T& paramValue)
        : cfgReader_{cfgReader}
        , paramName_{(sfunPrecondition(!paramName.empty()), std::move(paramName))}
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
        if (cfgReader_)
            cfgReader_->addValidator(std::make_unique<Validator<T>>(*param_, paramValue_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    ParamCreator<T>& ensure(TArgs&&... args)
    {
        if (cfgReader_)
            cfgReader_->addValidator(std::make_unique<Validator<T>>(*param_, paramValue_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

    operator T()
    {
        if (cfgReader_)
            cfgReader_->addParam(paramName_, std::move(param_));
        return defaultValue_;
    }

private:
    ConfigReaderPtr cfgReader_;
    std::string paramName_;
    T& paramValue_;
    std::unique_ptr<Param<T>> param_;
    T defaultValue_;
};

}