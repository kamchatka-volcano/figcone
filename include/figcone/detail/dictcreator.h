#pragma once
#include "iconfig.h"
#include "dict.h"
#include "gsl_assert.h"
#include <figcone/nameformat.h>
#include <memory>

namespace figcone::detail{

class DictCreator{
public:
    DictCreator(IConfig& cfg,
                std::string dictName,
                std::map<std::string, std::string>& dictMap)
        : cfg_{cfg}
        , dictName_{(Expects(!dictName.empty()), std::move(dictName))}
        , dict_{std::make_unique<Dict>(dictName_, dictMap)}
        , dictMap_{dictMap}
    {
    }

    DictCreator& operator()()
    {
        dict_->markValueIsSet();
        return *this;
    }

    operator std::map<std::string, std::string>()
    {
        cfg_.addNode(dictName_, std::move(dict_));
        return {};
    }

    DictCreator& checkedWith(std::function<void(const std::map<std::string, std::string>&)> validatingFunc)
    {
        cfg_.addValidator(
                std::make_unique<Validator<std::map<std::string, std::string>>>(*dict_, dictMap_,
                                                                                std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    DictCreator& checkedWith(TArgs&&... args)
    {
        cfg_.addValidator(
                std::make_unique<Validator<std::map<std::string, std::string>>>(*dict_, dictMap_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    IConfig& cfg_;
    std::string dictName_;
    std::unique_ptr<Dict> dict_;
    std::map<std::string, std::string>& dictMap_;
};

inline DictCreator makeDictCreator(IConfig& parentConfig,
                                   std::string dictName,
                                   const std::function<std::map<std::string, std::string>&()>& dictMapGetter)
{
    return {parentConfig, std::move(dictName), dictMapGetter()};
}


}