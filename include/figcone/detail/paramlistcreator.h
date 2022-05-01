#pragma once
#include "paramlist.h"
#include "iconfig.h"
#include "inode.h"
#include "validator.h"
#include "gsl_assert.h"
#include "utils.h"
#include <vector>

namespace figcone::detail{

template<typename TParamList>
class ParamListCreator{
public:
    ParamListCreator(IConfig& cfg,
                     std::string paramListName,
                     TParamList& paramListValue)
        : cfg_{cfg}
        , paramListName_{(Expects(!paramListName.empty()), std::move(paramListName))}
        , paramListValue_{paramListValue}
        , paramList_{std::make_unique<ParamList<TParamList>>(paramListName_, paramListValue)}
    {
        static_assert(is_sequence_container_v<maybe_opt_t<TParamList>>,
                      "Param list field must be a sequence container or a sequence container placed in std::optional");
    }

    ParamListCreator<TParamList>& operator()(TParamList defaultValue = {})
    {
        defaultValue_ = std::move(defaultValue);
        paramList_->markValueIsSet();
        return *this;
    }

    ParamListCreator<TParamList>& ensure(std::function<void(const TParamList&)> validatingFunc)
    {
        cfg_.addValidator(
                std::make_unique<Validator<TParamList>>(*paramList_, paramListValue_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    ParamListCreator<TParamList>& ensure(TArgs&&... args)
    {
        cfg_.addValidator(std::make_unique<Validator<TParamList>>(*paramList_, paramListValue_,
                                                                  TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

    operator TParamList()
    {
        cfg_.addParam(paramListName_, std::move(paramList_));
        return defaultValue_;
    }

private:
    IConfig& cfg_;
    std::string paramListName_;
    TParamList& paramListValue_;
    std::unique_ptr<ParamList<TParamList>> paramList_;
    TParamList defaultValue_;
};

template<typename TParamList>
ParamListCreator<TParamList> makeParamListCreator(IConfig& cfg,
                                                  std::string paramListName,
                                                  const std::function<TParamList&()>& paramGetter)
{
    static_assert(is_sequence_container_v<maybe_opt_t<TParamList>>,
                  "Param list field must be a sequence container or a sequence container placed in std::optional");
    return {cfg, std::move(paramListName), paramGetter()};
}

}