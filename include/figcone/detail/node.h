#pragma once
#include "iconfig.h"
#include "inode.h"
#include "iconfigentity.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <string>
#include <sstream>
#include <algorithm>

namespace figcone::detail {

class ConfigNode : public IConfigNode{
public:
    ConfigNode(std::string name, IConfig& cfg)
        : name_{std::move(name)}
        , cfg_{cfg}
    {
    }

private:
    void load(const figcone::TreeNode& node) override
    {
        hasValue_ = true;
        position_ = node.position();
        cfg_.load(node);
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
        return "Node '" + name_ + "'";
    }

private:
    std::string name_;
    IConfig& cfg_;
    bool hasValue_ = false;
    StreamPosition position_;
};

}