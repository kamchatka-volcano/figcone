#pragma once
#include "iconfig.h"
#include "dict.h"
#include "gsl_assert.h"
#include <figcone/nameformat.h>
#include <memory>

namespace figcone::detail{

template<typename TMap>
class DictCreator{
public:
    DictCreator(IConfig& cfg,
                std::string dictName,
                TMap& dictMap)
        : cfg_{cfg}
        , dictName_{(Expects(!dictName.empty()), std::move(dictName))}
        , dict_{std::make_unique<Dict<TMap>>(dictName_, dictMap)}
        , dictMap_{dictMap}
    {
        static_assert(is_associative_container_v<maybe_opt_t<TMap>>,
              "Dictionary field must be an associative container or an associative container placed in std::optional");
        static_assert(std::is_same_v<typename maybe_opt_t<TMap>::key_type, std::string>,
                     "Dictionary associative container's key type must be std::string");
    }

    DictCreator& operator()(TMap defaultValue = {})
    {
        dict_->markValueIsSet();
        defaultValue_ = std::move(defaultValue);
        return *this;
    }

    operator TMap()
    {
        cfg_.addNode(dictName_, std::move(dict_));
        return defaultValue_;
    }

    DictCreator& checkedWith(std::function<void(const TMap&)> validatingFunc)
    {
        cfg_.addValidator(
                std::make_unique<Validator<TMap>>(*dict_, dictMap_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    DictCreator& checkedWith(TArgs&&... args)
    {
        cfg_.addValidator(
                std::make_unique<Validator<TMap>>(*dict_, dictMap_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    IConfig& cfg_;
    std::string dictName_;
    std::unique_ptr<Dict<TMap>> dict_;
    TMap& dictMap_;
    TMap defaultValue_;
};

template<typename TMap>
inline DictCreator<TMap> makeDictCreator(IConfig& parentConfig,
                                         std::string dictName,
                                         const std::function<TMap&()>& dictMapGetter)
{
    static_assert(is_associative_container_v<maybe_opt_t<TMap>>,
                  "Dictionary field must be an associative container or an associative container placed in std::optional");
    static_assert(std::is_same_v<typename maybe_opt_t<TMap>::key_type, std::string>,
                  "Dictionary associative container's key type must be std::string");
    return {parentConfig, std::move(dictName), dictMapGetter()};
}


}