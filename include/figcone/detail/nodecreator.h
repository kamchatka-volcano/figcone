#pragma once
#include "node.h"
#include "inode.h"
#include "iconfigreader.h"
#include "validator.h"
#include "gsl_assert.h"
#include "utils.h"
#include <figcone/config.h>
#include <figcone/nameformat.h>

namespace figcone{
class Config;
}

namespace figcone::detail{

template<typename TCfg>
class NodeCreator{
    static_assert(std::is_base_of_v<Config, remove_optional_t<TCfg>>,
            "TConfig must be a subclass of figcone::Config.");
public:
    NodeCreator(ConfigReaderPtr cfgReader,
                std::string nodeName,
                TCfg& nodeCfg)
        : cfgReader_{cfgReader}
        , nodeName_{(Expects(!nodeName.empty()), std::move(nodeName))}
        , nodeCfg_{nodeCfg}
        , nestedCfgReader_{ cfgReader_ ? cfgReader_->makeNestedReader(nodeName_) : ConfigReaderPtr{}}
    {
        if constexpr(is_optional_v<TCfg>)
            static_assert(dependent_false<TCfg>,"TConfig can't be placed in std::optional, use figcone::optional instead.");
        node_ = std::make_unique<Node<TCfg>>(nodeName_, nodeCfg_, nestedCfgReader_);
    }

    NodeCreator<TCfg>& operator()()
    {
        node_->markValueIsSet();
        return *this;
    }

    operator TCfg()
    {
        if (cfgReader_)
            cfgReader_->addNode(nodeName_, std::move(node_));
         if constexpr(is_optional_config_field<TCfg>::value)
            return TCfg{typename TCfg::value_type{nestedCfgReader_}};
         else
            return TCfg{nestedCfgReader_};
    }

    NodeCreator<TCfg>& ensure(std::function<void(const TCfg&)> validatingFunc)
    {
        if (cfgReader_)
            cfgReader_->addValidator(std::make_unique<Validator<TCfg>>(*node_, nodeCfg_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    NodeCreator<TCfg>& ensure(TArgs&&... args)
    {
        if (cfgReader_)
            cfgReader_->addValidator(
                    std::make_unique<Validator<TCfg>>(*node_, nodeCfg_, TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    ConfigReaderPtr cfgReader_;
    std::string nodeName_;
    TCfg& nodeCfg_;
    ConfigReaderPtr nestedCfgReader_;
    std::unique_ptr<Node<TCfg>> node_;
};

}