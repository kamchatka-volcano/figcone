#pragma once
#include "iconfig.h"
#include "nodelist.h"
#include "gsl_assert.h"
#include <figcone/nameformat.h>

namespace figcone::detail{

template<typename TCfg>
class NodeListCreator{
public:
    NodeListCreator(IConfig& cfg,
                    std::string nodeListName,
                    std::vector<TCfg>& nodeList,
                    NodeListType type = NodeListType::Normal)
        : cfg_{cfg}
        , nodeListName_{(Expects(!nodeListName.empty()), std::move(nodeListName))}
        , nodeList_{std::make_unique<NodeList<TCfg>>(nodeListName_, nodeList, type)}
        , nodeListValue_(nodeList)
    {
        static_assert(std::is_base_of_v<IConfig, TCfg>, "TNode must be a subclass of figcone::INode.");
    }

    NodeListCreator<TCfg>& operator()()
    {
        nodeList_->markValueIsSet();
        return *this;
    }

    operator std::vector<TCfg>()
    {
        cfg_.addNode(nodeListName_, std::move(nodeList_));
        return {};
    }

    NodeListCreator<TCfg>& ensure(std::function<void(const std::vector<TCfg>&)> validatingFunc)
    {
        cfg_.addValidator(
                std::make_unique<Validator<std::vector<TCfg>>>(*nodeList_, nodeListValue_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    NodeListCreator<TCfg>& ensure(TArgs&&... args)
    {
        cfg_.addValidator(std::make_unique<Validator<std::vector<TCfg>>>(*nodeList_, nodeListValue_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    IConfig& cfg_;
    std::string nodeListName_;
    std::unique_ptr<NodeList<TCfg>> nodeList_;
    std::vector<TCfg>& nodeListValue_;
};

template<typename TCfg, typename TParentCfg>
NodeListCreator <TCfg> makeNodeListCreator(TParentCfg& parentCfg,
                                           std::string nodeListName,
                                           const std::function<std::vector<TCfg>&()>& nodeListGetter,
                                           NodeListType type = NodeListType::Normal)
{
    static_assert(TCfg::format() == TParentCfg::format(),
                  "Node's config type must have the same name format as its parent.");
    return {parentCfg, std::move(nodeListName), nodeListGetter(), type};
}


}