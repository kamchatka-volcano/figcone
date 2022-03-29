#pragma once
#include "inode.h"
#include "param.h"
#include <figcone_tree/tree.h>
#include <map>

namespace figcone::detail {

class ConfigDict : public IConfigNode{
public:
    explicit ConfigDict(std::string name, std::map<std::string, std::string>& dictMap)
        : name_{std::move(name)}
        , dictMap_{dictMap}
    {}

private:
    void load(const figcone::TreeNode& node) override
    {
        hasValue_ = true;
        position_ = node.position();
        for (const auto& [paramName, paramValue] : node.asItem().params())
           dictMap_[paramName] = paramValue.value();
    }

    bool hasValue() const override
    {
        return hasValue_;
    }

    StreamPosition position() override
    {
        return position_;
    }

    std::string description() override
    {
        return "Dictionary '" + name_ + "'";
    }

private:
    std::string name_;
    std::map<std::string, std::string>& dictMap_;
    bool hasValue_ = false;
    StreamPosition position_;
};

}