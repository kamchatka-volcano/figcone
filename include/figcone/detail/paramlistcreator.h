#pragma once
#include "paramlist.h"
#include "iconfig.h"
#include "inode.h"
#include "validator.h"
#include "gsl_assert.h"
#include <vector>

namespace figcone::detail{

template<typename T>
class ConfigParamListCreator{
public:
    ConfigParamListCreator(IConfig& cfg,
                           std::string paramListName,
                           std::vector<T>& paramListValue)
        : cfg_{cfg}
        , paramListName_{(Expects(!paramListName.empty()), std::move(paramListName))}
        , paramListValue_{paramListValue}
        , paramList_{std::make_unique<ConfigParamList<T>>(paramListName_, paramListValue)}
    {
    }

    ConfigParamListCreator<T>& operator()(std::vector<T> defaultValue = {})
    {
        defaultValue_ = std::move(defaultValue);
        paramList_->markValueIsSet();
        return *this;
    }

    ConfigParamListCreator<T>& checkedWith(std::function<void(const std::vector<T>&)> validatingFunc)
    {
        cfg_.addValidator(
                std::make_unique<Validator<std::vector<T>>>(*paramList_, paramListValue_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    ConfigParamListCreator<T>& checkedWith(TArgs&&... args)
    {
        cfg_.addValidator(std::make_unique<Validator<std::vector<T>>>(*paramList_, paramListValue_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

    operator std::vector<T>()
    {
        cfg_.addParam(paramListName_, std::move(paramList_));
        return defaultValue_;
    }

private:
    IConfig& cfg_;
    std::string paramListName_;
    std::vector<T>& paramListValue_;
    std::unique_ptr<ConfigParamList<T>> paramList_;
    std::vector<T> defaultValue_;
};

template<typename T>
ConfigParamListCreator<T> makeParamListCreator(IConfig& cfg,
                                               std::string paramListName,
                                               const std::function<std::vector<T>&()>& paramGetter)
{
    return {cfg, std::move(paramListName), paramGetter()};
}

}