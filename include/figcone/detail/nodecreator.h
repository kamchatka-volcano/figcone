#pragma once
#include "node.h"
#include "inode.h"
#include "iconfig.h"
#include "validator.h"
#include "gsl_assert.h"
#include "utils.h"
#include <figcone/nameformat.h>

namespace figcone::detail{

template<typename TCfg>
class NodeCreator{
public:
    NodeCreator(IConfig& cfg,
                std::string nodeName,
                TCfg& nodeCfg)
        : cfg_{cfg}
        , nodeName_{(Expects(!nodeName.empty()), std::move(nodeName))}
        , nodeCfg_{nodeCfg}
    {
        if constexpr (detail::is_optional<TCfg>::value) {
            static_assert(std::is_base_of_v<IConfig, typename TCfg::value_type>,
                          "TConfig must be a subclass of figcone::IConfig.");
            }
        else {
            static_assert(std::is_base_of_v<IConfig, TCfg>, "TConfig must be a subclass of figcone::IConfig.");

        }
        node_ = std::make_unique<Node<TCfg>>(nodeName_, nodeCfg_);
    }

    NodeCreator<TCfg>& operator()()
    {
        node_->markValueIsSet();
        return *this;
    }

    operator TCfg()
    {
        cfg_.addNode(nodeName_, std::move(node_));
        return {};
    }

    NodeCreator<TCfg>& ensure(std::function<void(const TCfg&)> validatingFunc)
    {
        cfg_.addValidator(std::make_unique<Validator<TCfg>>(*node_, nodeCfg_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    NodeCreator<TCfg>& ensure(TArgs&&... args)
    {
        cfg_.addValidator(std::make_unique<Validator<TCfg>>(*node_, nodeCfg_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    IConfig& cfg_;
    std::string nodeName_;
    TCfg& nodeCfg_;
    std::unique_ptr<Node<TCfg>> node_;
};

template<typename TCfg, typename TParentCfg>
NodeCreator<TCfg> makeNodeCreator(TParentCfg& parentCfg,
                                  std::string nodeName,
                                  const std::function<TCfg&()>& configGetter)
{
    if constexpr (detail::is_optional<TCfg>::value)
        static_assert(TCfg::value_type::format() == TParentCfg::format(),
                  "Node's config type must have the same name format as its parent.");
    else
        static_assert(TCfg::format() == TParentCfg::format(),
                  "Node's config type must have the same name format as its parent.");
    return {parentCfg, std::move(nodeName), configGetter()};
}


}