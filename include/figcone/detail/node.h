#pragma once
#include "iconfigreader.h"
#include "inode.h"
#include "iconfigentity.h"
#include "iconfigreader.h"
#include "utils.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <string>
#include <sstream>
#include <algorithm>

namespace figcone::detail {

template<typename TCfg>
class Node : public INode{
public:
    Node(std::string name, TCfg& cfg, ConfigReaderPtr cfgReader)
        : name_{std::move(name)}
        , cfg_{cfg}
        , cfgReader_{cfgReader}
    {
    }

    void markValueIsSet()
    {
        hasValue_ = true;
    }

private:
    void load(const TreeNode& node) override
    {
        hasValue_ = true;
        position_ = node.position();
        if (!node.isItem())
           throw ConfigError{"Node '" + name_ + "': config node can't be a list.", node.position()};

        if constexpr (is_initialized_optional_v<TCfg>)
            cfg_.emplace();

        if (cfgReader_)
            cfgReader_->load(node);
    }

    bool hasValue() const override
    {
        if constexpr (is_initialized_optional_v<TCfg>)
            return true;
        else
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
    TCfg& cfg_;
    ConfigReaderPtr cfgReader_;
    bool hasValue_ = false;
    StreamPosition position_;
};

}