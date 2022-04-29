#pragma once
#include "iconfig.h"
#include "inode.h"
#include "iconfigentity.h"
#include "utils.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <string>
#include <sstream>
#include <algorithm>

namespace figcone::detail {

template<typename TCfg>
class ConfigNode : public IConfigNode{
public:
    ConfigNode(std::string name, TCfg& cfg)
        : name_{std::move(name)}
        , cfg_{cfg}
    {
    }

private:
    void load(const figcone::TreeNode& node) override
    {
        hasValue_ = true;
        position_ = node.position();
        if (!node.isItem())
           throw ConfigError{"Node '" + name_ + "': config node can't be a list.", node.position()};

        if constexpr (detail::is_optional<TCfg>::value) {
            cfg_.emplace();
            auto& icfg = static_cast<IConfig&>(*cfg_);
            icfg.load(node);
        }
        else{
            auto& icfg = static_cast<IConfig&>(cfg_);
            icfg.load(node);
        }
    }

    bool hasValue() const override
    {
        if constexpr (detail::is_optional<TCfg>::value)
            return true;

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
    bool hasValue_ = false;
    StreamPosition position_;
};

}