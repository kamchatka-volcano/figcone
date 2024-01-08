#ifndef FIGCONE_PARAMCREATOR_H
#define FIGCONE_PARAMCREATOR_H

#include "configreaderaccess.h"
#include "param.h"
#include "validator.h"
#include "external/sfun/contract.h"

namespace figcone::detail {

template<typename T>
class ParamCreator {
public:
    ParamCreator(ConfigReaderPtr cfgReader, std::string paramName, T& paramValue, bool isOptional = false)
        : cfgReader_{cfgReader}
        , paramName_{(sfun_precondition(!paramName.empty()), std::move(paramName))}
        , paramValue_{paramValue}
        , param_{std::make_unique<Param<T>>(paramName_, paramValue)}
    {
        if (isOptional)
            param_->markValueIsSet();
    }

    ParamCreator<T>& operator()(T defaultValue = {})
    {
        defaultValue_ = std::move(defaultValue);
        param_->markValueIsSet();
        return *this;
    }

    ParamCreator<T>& ensure(std::function<void(const sfun::remove_optional_t<T>&)> validatingFunc)
    {
        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.addValidator(
                    std::make_unique<Validator<T>>(*param_, paramValue_, std::move(validatingFunc)));
        return *this;
    }

    template<typename TValidator, typename... TArgs>
    ParamCreator<T>& ensure(TArgs&&... args)
    {
        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.addValidator(
                    std::make_unique<Validator<T>>(*param_, paramValue_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

    void createParam()
    {
        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.addParam(paramName_, std::move(param_));
    }

    operator T()
    {
        createParam();
        return defaultValue_;
    }

private:
    ConfigReaderPtr cfgReader_;
    std::string paramName_;
    T& paramValue_;
    std::unique_ptr<Param<T>> param_;
    T defaultValue_;
};

} //namespace figcone::detail

#endif //FIGCONE_PARAMCREATOR_H