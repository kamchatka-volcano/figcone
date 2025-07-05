#ifndef FIGCONE_NODELISTCREATOR_H
#define FIGCONE_NODELISTCREATOR_H

#include "configreaderaccess.h"
#include "creatormode.h"
#include "nodelist.h"
#include "external/eel/contract.h"
#include "external/eel/type_traits.h"
#include <figcone/nameformat.h>

namespace figcone {
class Config;
}

namespace figcone::detail {

template<typename TCfgList, CreatorMode creatorMode = CreatorMode::RuntimeReflection>
class NodeListCreator {
    static_assert(
            eel::is_dynamic_sequence_container_v<eel::remove_optional_t<TCfgList>>,
            "Node list field must be a sequence container or a sequence container placed in std::optional");
    static_assert(
            std::conditional_t<
                    creatorMode == CreatorMode::RuntimeReflection,
                    std::is_base_of<Config, typename eel::remove_optional_t<TCfgList>::value_type>,
                    std::true_type>::value,
            "TConfig must be a subclass of figcone::Config.");
    static_assert(
            std::conditional_t<
                    creatorMode == CreatorMode::StaticReflection,
                    std::negation<std::is_base_of<Config, typename eel::remove_optional_t<TCfgList>::value_type>>,
                    std::true_type>::value,
            "TConfig must not be a subclass of figcone::Config when static reflection interface is used.");

public:
    NodeListCreator(
            ConfigReaderPtr cfgReader,
            std::string nodeListName,
            TCfgList& nodeList,
            NodeListType type = NodeListType::Normal,
            bool isOptional = false)
        : cfgReader_{cfgReader}
        , nodeListName_{(eel::precondition(!nodeListName.empty(), FIGCONE_EEL_LINE), std::move(nodeListName))}
        , nodeList_{std::make_unique<NodeList<TCfgList>>(
                  nodeListName_,
                  nodeList,
                  cfgReader_ ? ConfigReaderAccess{cfgReader_}.makeNestedReader(nodeListName_) : ConfigReaderPtr{},
                  type)}
        , nodeListValue_(nodeList)
    {
        if (isOptional)
            nodeList_->markValueIsSet();
    }

    NodeListCreator& operator()()
    {
        nodeList_->markValueIsSet();
        return *this;
    }

    void createNodeList()
    {
        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.addNode(nodeListName_, std::move(nodeList_));
    }

    operator TCfgList()
    {
        createNodeList();
        return {};
    }

    NodeListCreator& ensure(std::function<void(const eel::remove_optional_t<TCfgList>&)> validatingFunc)
    {
        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.addValidator(
                    std::make_unique<Validator<TCfgList>>(*nodeList_, nodeListValue_, std::move(validatingFunc)));
        return *this;
    }

    template<typename TValidator, typename... TArgs>
    NodeListCreator& ensure(TArgs&&... args)
    {
        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.addValidator(std::make_unique<Validator<TCfgList>>(
                    *nodeList_,
                    nodeListValue_,
                    TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    ConfigReaderPtr cfgReader_;
    std::string nodeListName_;
    std::unique_ptr<NodeList<TCfgList>> nodeList_;
    TCfgList& nodeListValue_;
};

} //namespace figcone::detail

#endif //FIGCONE_NODELISTCREATOR_H