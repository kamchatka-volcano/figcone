#pragma once
#include "inode.h"
#include "loadingerror.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <vector>
#include <memory>

namespace figcone::detail{

template <typename TCfg>
class ConfigNodeList : public detail::IConfigNode{
public:
    ConfigNodeList(std::string name, std::vector<TCfg>& nodeList)
        : name_{std::move(name)}
        , nodeList_{nodeList}

    {}

    void load(const figcone::TreeNode& nodeList) override
    {
        hasValue_ = true;
        position_ = nodeList.position();
        for (auto i = 0; i < nodeList.asList().count(); ++i){
            const auto& treeNode = nodeList.asList().node(i);
            try {
                auto cfg = TCfg{};
                auto& node = static_cast<IConfig&>(cfg);
                node.load(treeNode);
                nodeList_.emplace_back(std::move(cfg));
            } catch (const LoadingError& e) {
                throw ConfigError{treeNode.position(), "Node list '" + name_ + "': " + e.what()};
            }
        }

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
        return "Node list '" + name_ + "'";
    }

private:
    std::string name_;
    std::vector<TCfg>& nodeList_;
    bool hasValue_ = false;
    StreamPosition position_;
};

}