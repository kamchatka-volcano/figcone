#pragma once
#include <figcone_tree/tree.h>
#include <memory>
#include <string>

namespace figcone::detail {
class IConfigNode;
class IConfigParam;
class IValidator;


class IConfig{
public:
    virtual ~IConfig() = default;
    virtual void load(const figcone::TreeNode& node) = 0;
    virtual void addNode(const std::string& name, std::unique_ptr<IConfigNode> cfg) = 0;
    virtual void addParam(const std::string& name, std::unique_ptr<IConfigParam> param) = 0;
    virtual void addValidator(std::unique_ptr<IValidator> validator) = 0;
};

}