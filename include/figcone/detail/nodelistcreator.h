#pragma once
#include "iconfig.h"
#include "nodelist.h"
#include "gsl_assert.h"
#include <figcone/nameformat.h>

namespace figcone::detail{

template<typename TCfgList>
class NodeListCreator{
public:
    NodeListCreator(IConfig& cfg,
                    std::string nodeListName,
                    TCfgList& nodeList,
                    NodeListType type = NodeListType::Normal)
        : cfg_{cfg}
        , nodeListName_{(Expects(!nodeListName.empty()), std::move(nodeListName))}
        , nodeList_{std::make_unique<NodeList<TCfgList>>(nodeListName_, nodeList, type)}
        , nodeListValue_(nodeList)
    {
        static_assert(is_sequence_container_v<maybe_opt_t<TCfgList>>,
                      "Node list field must be a sequence container or a sequence container placed in std::optional");
        static_assert(std::is_base_of_v<IConfig, typename maybe_opt_t<TCfgList>::value_type>,
                      "TNode must be a subclass of figcone::INode.");
    }

    NodeListCreator<TCfgList>& operator()()
    {
        nodeList_->markValueIsSet();
        return *this;
    }

    operator TCfgList()
    {
        cfg_.addNode(nodeListName_, std::move(nodeList_));
        return {};
    }

    NodeListCreator<TCfgList>& ensure(std::function<void(const TCfgList&)> validatingFunc)
    {
        cfg_.addValidator(
                std::make_unique<Validator<TCfgList>>(*nodeList_, nodeListValue_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    NodeListCreator<TCfgList>& ensure(TArgs&&... args)
    {
        cfg_.addValidator(std::make_unique<Validator<TCfgList>>(*nodeList_, nodeListValue_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    IConfig& cfg_;
    std::string nodeListName_;
    std::unique_ptr<NodeList<TCfgList>> nodeList_;
    TCfgList& nodeListValue_;
};

template<typename TCfgList, typename TParentCfg>
NodeListCreator <TCfgList> makeNodeListCreator(TParentCfg& parentCfg,
                                               std::string nodeListName,
                                               const std::function<TCfgList&()>& nodeListGetter,
                                               NodeListType type = NodeListType::Normal)
{
    static_assert(is_sequence_container_v<maybe_opt_t<TCfgList>>,
              "Node list field must be a sequence container or a sequence container placed in std::optional");
    static_assert(maybe_opt_t<TCfgList>::value_type::format() == TParentCfg::format(),
                  "Node's config type must have the same name format as its parent.");
    return {parentCfg, std::move(nodeListName), nodeListGetter(), type};
}


}