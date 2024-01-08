#ifndef FIGCONE_NODECREATOR_H
#define FIGCONE_NODECREATOR_H

#include "configreaderaccess.h"
#include "creatormode.h"
#include "inode.h"
#include "node.h"
#include "utils.h"
#include "validator.h"
#include "external/sfun/contract.h"
#include "external/sfun/type_traits.h"
#include "external/sfun/utility.h"
#include <figcone/nameformat.h>
#include <type_traits>

namespace figcone {
class Config;
}

namespace figcone::detail {

template<typename TCfg, CreatorMode creatorMode = CreatorMode::RuntimeReflection>
class NodeCreator {
    static_assert(
            std::conditional_t<
                    creatorMode == CreatorMode::RuntimeReflection,
                    std::is_base_of<Config, sfun::remove_optional_t<TCfg>>,
                    std::true_type>::value,
            "TConfig must be a subclass of figcone::Config.");
    static_assert(
            std::conditional_t<
                    creatorMode == CreatorMode::StaticReflection,
                    std::negation<std::is_base_of<Config, sfun::remove_optional_t<TCfg>>>,
                    std::true_type>::value,
            "TConfig must not inherit from figcone::Config when static reflection interface is used.");

public:
    NodeCreator(ConfigReaderPtr cfgReader, std::string nodeName, TCfg& nodeCfg, bool isOptional = false)
        : cfgReader_{cfgReader}
        , nodeName_{(sfun_precondition(!nodeName.empty()), std::move(nodeName))}
        , nodeCfg_{nodeCfg}
        , nestedCfgReader_{cfgReader_ ? ConfigReaderAccess{cfgReader_}.makeNestedReader(nodeName_) : ConfigReaderPtr{}}
    {
        if constexpr (std::is_base_of_v<figcone::Config, TCfg> && sfun::is_optional_v<TCfg>)
            static_assert(
                    sfun::dependent_false<TCfg>,
                    "TConfig can't be placed in std::optional, use figcone::optional instead.");
        node_ = std::make_unique<Node<TCfg>>(nodeName_, nodeCfg_, nestedCfgReader_);

        if (isOptional)
            node_->markValueIsSet();
    }

    NodeCreator& operator()()
    {
        node_->markValueIsSet();
        return *this;
    }

    void createNode()
    {
        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.addNode(nodeName_, std::move(node_));
    }

    operator TCfg()
    {
        createNode();

        if constexpr (!std::is_aggregate_v<TCfg>)
            static_assert(
                    std::is_constructible_v<TCfg, detail::ConfigReaderPtr>,
                    "Non aggregate config objects must inherit figcone::Config constructors with 'using "
                    "Config::Config;'");

        return TCfg{nestedCfgReader_};
    }

    NodeCreator& ensure(std::function<void(const sfun::remove_optional_t<TCfg>&)> validatingFunc)
    {
        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.addValidator(
                    std::make_unique<Validator<TCfg>>(*node_, nodeCfg_, std::move(validatingFunc)));
        return *this;
    }

    template<typename TValidator, typename... TArgs>
    NodeCreator& ensure(TArgs&&... args)
    {
        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.addValidator(
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

} //namespace figcone::detail

#endif //FIGCONE_NODECREATOR_H