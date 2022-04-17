#pragma once
#include "node.h"
#include "inode.h"
#include "iconfig.h"
#include "validator.h"
#include "gsl_assert.h"
#include <figcone/nameformat.h>

namespace figcone::detail{

template<typename TCfg>
class ConfigNodeCreator{
public:
    ConfigNodeCreator(IConfig& cfg,
                      std::string nodeName,
                      TCfg& nodeCfg)
    : cfg_{cfg}
    , nodeName_{(Expects(!nodeName.empty()), std::move(nodeName))}
    , nodeCfg_{nodeCfg}
    , node_{std::make_unique<ConfigNode>(nodeName_, nodeCfg_)}
    {
        static_assert(std::is_base_of_v<IConfig, TCfg>, "TConfig must be a subclass of figcone::IConfig.");
    }

    operator TCfg()
    {
        cfg_.addNode(nodeName_, std::move(node_));
        return {};
    }

    ConfigNodeCreator<TCfg>& ensure(std::function<void(const TCfg&)> validatingFunc)
    {
        cfg_.addValidator(std::make_unique<Validator<TCfg>>(*node_, nodeCfg_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    ConfigNodeCreator<TCfg>& ensure(TArgs&&... args)
    {
        cfg_.addValidator(std::make_unique<Validator<TCfg>>(*node_, nodeCfg_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    IConfig& cfg_;
    std::string nodeName_;
    TCfg& nodeCfg_;
    std::unique_ptr<ConfigNode> node_;
};

template<typename TCfg, typename TParentCfg>
ConfigNodeCreator<TCfg> makeNodeCreator(TParentCfg& parentCfg,
                                        std::string nodeName,
                                        const std::function<TCfg&()>& configGetter)
{
    static_assert(TCfg::format() == TParentCfg::format(),
                  "ConfigNode's config type must have the same name format as its parent.");
    return {parentCfg, std::move(nodeName), configGetter()};
}


}