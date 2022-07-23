#pragma once
#include "configreaderstorage.h"
#include "configreaderptr.h"
#include <figcone_tree/tree.h>
#include <memory>
#include <string>

namespace figcone::detail {
class INode;
class IParam;
class IValidator;
class IConfigReader;

class IConfigReader{
public:
    virtual ~IConfigReader() = default;
    virtual ConfigReaderPtr makeNestedReader(const std::string& name) = 0;
    virtual void load(const figcone::TreeNode& node) = 0;
    virtual void addNode(const std::string& name, std::unique_ptr<INode> cfg) = 0;
    virtual void addParam(const std::string& name, std::unique_ptr<IParam> param) = 0;
    virtual void addValidator(std::unique_ptr<IValidator> validator) = 0;
    virtual void swapContents(ConfigReaderPtr) = 0;

protected:
    ConfigReaderPtr makePtr()
    {
        return this;
    }

    void clearConfigReaderStorage(ConfigReaderStorage& storage)
    {
        storage.cfgReader_ = ConfigReaderPtr{};
    }
};

}