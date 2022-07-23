#pragma once
#include "iconfigreader.h"
#include "nodelist.h"
#include "gsl_assert.h"
#include <figcone/nameformat.h>

namespace figcone::detail{

template<typename TCfgList>
class NodeListCreator{
    static_assert(is_sequence_container_v<remove_optional_t<TCfgList>>,
            "Node list field must be a sequence container or a sequence container placed in std::optional");
    static_assert(std::is_base_of_v<ConfigReaderStorage, typename remove_optional_t<TCfgList>::value_type>,
            "TCfgList::value_type must be a subclass of figcone::Config.");
public:
    NodeListCreator(ConfigReaderPtr cfgReader,
                    std::string nodeListName,
                    TCfgList& nodeList,
                    NodeListType type = NodeListType::Normal)
        : cfgReader_{cfgReader}
        , nodeListName_{(Expects(!nodeListName.empty()), std::move(nodeListName))}
        , nodeList_{std::make_unique<NodeList<TCfgList>>(nodeListName_, nodeList,
                                                         cfgReader_ ? cfgReader_->makeNestedReader(nodeListName_)
                                                                    : ConfigReaderPtr{}, type)}
        , nodeListValue_(nodeList)
    {
    }

    NodeListCreator<TCfgList>& operator()()
    {
        nodeList_->markValueIsSet();
        return *this;
    }

    operator TCfgList()
    {
        if (cfgReader_)
            cfgReader_->addNode(nodeListName_, std::move(nodeList_));
        return {};
    }

    NodeListCreator<TCfgList>& ensure(std::function<void(const TCfgList&)> validatingFunc)
    {
        if (cfgReader_)
            cfgReader_->addValidator(
                    std::make_unique<Validator<TCfgList>>(*nodeList_, nodeListValue_, std::move(validatingFunc)));
        return *this;
    }

    template <typename TValidator, typename... TArgs>
    NodeListCreator<TCfgList>& ensure(TArgs&&... args)
    {
        if (cfgReader_)
            cfgReader_->addValidator(std::make_unique<Validator<TCfgList>>(*nodeList_, nodeListValue_,
                                                                           TValidator{std::forward<TArgs>(args)...}));
        return *this;
    }

private:
    ConfigReaderPtr cfgReader_;
    std::string nodeListName_;
    std::unique_ptr<NodeList<TCfgList>> nodeList_;
    TCfgList& nodeListValue_;
    ConfigReaderPtr nestedCfgReader_;
};

template<typename TCfgList>
NodeListCreator <TCfgList> makeNodeListCreator(ConfigReaderPtr cfgReader,
                                               std::string nodeListName,
                                               const std::function<TCfgList&()>& nodeListGetter,
                                               NodeListType type = NodeListType::Normal)
{
    return {cfgReader, std::move(nodeListName), nodeListGetter(), type};
}


}