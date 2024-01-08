#ifndef FIGCONE_CONFIGREADERACCESS_H
#define FIGCONE_CONFIGREADERACCESS_H

#include "configreaderptr.h"
#include <memory>
#include <string>

namespace figcone {
class TreeNode;
};

namespace figcone::detail {
class INode;
class IParam;
class IValidator;

template<typename TConfigReaderPtr>
class ConfigReaderAccess {
public:
    ConfigReaderAccess(TConfigReaderPtr configReader)
        : configReader_{configReader}
    {
    }

    void addNode(const std::string& name, std::unique_ptr<detail::INode> node)
    {
        configReader_->addNode(name, std::move(node));
    }

    void addParam(const std::string& name, std::unique_ptr<detail::IParam> param)
    {
        configReader_->addParam(name, std::move(param));
    }

    void addValidator(std::unique_ptr<detail::IValidator> validator)
    {
        configReader_->addValidator(std::move(validator));
    }

    template<typename TCfg>
    void load(const TreeNode& treeNode)
    {
        configReader_->template load<TCfg>(treeNode);
    }

    template<typename TCfg>
    void loadStructure(TCfg& cfg)
    {
        configReader_->template loadStructure<TCfg>(cfg);
    }

    detail::ConfigReaderPtr makeNestedReader(const std::string& name)
    {
        return configReader_->makeNestedReader(name);
    }

private:
    TConfigReaderPtr configReader_;
};

} //namespace figcone::detail

#endif //FIGCONE_CONFIGREADERACCESS_H