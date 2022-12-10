#pragma once
#include "configreaderptr.h"
#include "external/sfun/interface.h"
#include <figcone_tree/tree.h>
#include <memory>
#include <string>

namespace figcone::detail {
class INode;
class IParam;
class IValidator;

class IConfigReader : private sfun::Interface<IConfigReader>{
public:
    virtual ConfigReaderPtr makeNestedReader(const std::string& name) = 0;
    virtual void load(const figcone::TreeNode& node) = 0;
    virtual void addNode(const std::string& name, std::unique_ptr<INode> cfg) = 0;
    virtual void addParam(const std::string& name, std::unique_ptr<IParam> param) = 0;
    virtual void addValidator(std::unique_ptr<IValidator> validator) = 0;

protected:
    ConfigReaderPtr makePtr()
    {
        return this;
    }

    template<typename TCfg>
    void resetConfigReader(TCfg& cfg)
    {
        cfg.cfgReader_ = ConfigReaderPtr{};
    }
};

}