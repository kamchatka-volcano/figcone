#pragma once
#include "iconfig.h"
#include "nodelist.h"
#include "gsl_assert.h"
#include <figcone/nameformat.h>

namespace figcone::detail{

template<typename TCfg>
class ConfigNodeListCreator{
public:
    ConfigNodeListCreator(IConfig& cfg,
                          std::string nodeListName,
                          std::vector<TCfg>& nodeList,
                          NodeListType type = NodeListType::Normal)
        : cfg_{cfg}
        , nodeListName_{(Expects(!nodeListName.empty()), std::move(nodeListName))}
        , nodeList_{std::make_unique<ConfigNodeList<TCfg>>(nodeListName_, nodeList, type)}
        , nodeListValue_(nodeList)
    {
        static_assert(std::is_base_of_v<IConfig, TCfg>, "TNode must be a subclass of figcone::IConfigNode.");
    }

    operator std::vector<TCfg>()
    {
        cfg_.addNode(nodeListName_, std::move(nodeList_));
        return {};
    }

    ConfigNodeListCreator<TCfg>& ensure(std::function<void(const std::vector<TCfg>&)> validatingFunc)
    {
        cfg_.addValidator(
                std::make_unique<Validator<std::vector<TCfg>>>(*nodeList_, nodeListValue_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    ConfigNodeListCreator<TCfg>& ensure(TArgs&&... args)
    {
        cfg_.addValidator(std::make_unique<Validator<std::vector<TCfg>>>(*nodeList_, nodeListValue_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    IConfig& cfg_;
    std::string nodeListName_;
    std::unique_ptr<ConfigNodeList<TCfg>> nodeList_;
    std::vector<TCfg>& nodeListValue_;
};

template<typename TCfg, typename TParentCfg>
ConfigNodeListCreator <TCfg> makeNodeListCreator(TParentCfg& parentCfg,
                                                 std::string nodeListName,
                                                 const std::function<std::vector<TCfg>&()>& nodeListGetter,
                                                 NodeListType type = NodeListType::Normal)
{
    static_assert(TCfg::format() == TParentCfg::format(),
                  "ConfigNode's config type must have the same name format as its parent.");
    return {parentCfg, std::move(nodeListName), nodeListGetter(), type};
}


}