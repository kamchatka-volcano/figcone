#pragma once
#include "inode.h"
#include "loadingerror.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <vector>
#include <memory>

namespace figcone::detail{

enum class NodeListType{
    Normal,
    Copy
};

template <typename TCfg>
class NodeList : public detail::INode{
public:
    NodeList(std::string name, std::vector<TCfg>& nodeList, NodeListType type = NodeListType::Normal)
        : name_{std::move(name)}
        , nodeList_{nodeList}
        , type_{type}
    {}

    void markValueIsSet()
    {
        hasValue_ = true;
    }


    void load(const TreeNode& nodeList) override
    {
        hasValue_ = true;
        position_ = nodeList.position();
        nodeList_.clear();
        if (!nodeList.isList())
            throw ConfigError{"Node list '" + name_ + "': config node must be a list.", nodeList.position()};

        for (auto i = 0; i < nodeList.asList().count(); ++i){
            const auto& treeNode = nodeList.asList().node(i);
            try {
                auto cfg = TCfg{};
                auto& node = static_cast<IConfig&>(cfg);
                if (type_ == NodeListType::Copy && i > 0)
                    node.load(nodeList.asList().node(0));
                node.load(treeNode);
                nodeList_.emplace_back(std::move(cfg));
            } catch (const LoadingError& e) {
                throw ConfigError{"Node list '" + name_ + "': " + e.what(), treeNode.position()};
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
    NodeListType type_;
};

}