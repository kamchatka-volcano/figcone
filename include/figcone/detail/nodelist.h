#pragma once
#include "inode.h"
#include "loadingerror.h"
#include "utils.h"
#include "external/sfun/traits.h"
#include <figcone_tree/tree.h>
#include <figcone/errors.h>
#include <vector>
#include <memory>
#include <type_traits>

namespace figcone::detail{
using namespace sfun::traits;

enum class NodeListType{
    Normal,
    Copy
};

template <typename TCfgList>
class NodeList : public detail::INode{
public:
    NodeList(std::string name, TCfgList& nodeList, ConfigReaderPtr cfgReader, NodeListType type = NodeListType::Normal)
        : name_{std::move(name)}
        , nodeList_{nodeList}
        , type_{type}
        , cfgReader_{cfgReader}
    {
        static_assert(is_dynamic_sequence_container_v<remove_optional_t<TCfgList>>,
                      "Node list field must be a sequence container or a sequence container placed in std::optional");
    }

    void markValueIsSet()
    {
        hasValue_ = true;
    }


    void load(const TreeNode& nodeList) override
    {
        hasValue_ = true;
        position_ = nodeList.position();
        if (!nodeList.isList())
            throw ConfigError{"Node list '" + name_ + "': config node must be a list.", nodeList.position()};
        if constexpr(is_optional<TCfgList>::value)
            nodeList_.emplace();

        maybeOptValue(nodeList_).clear();
        for (auto i = 0; i < nodeList.asList().count(); ++i){
            const auto& treeNode = nodeList.asList().node(i);
            try {
                auto cfg = [this]{
                    using CfgList = typename remove_optional_t<TCfgList>::value_type;
                    if constexpr (std::is_aggregate_v<CfgList>)
                        return CfgList{{cfgReader_}};
                    else{
                        static_assert(std::is_constructible_v<CfgList, detail::ConfigReaderPtr>, "Non aggregate config objects must inherit figcone::Config constructors with 'using Config::Config;'");
                        return CfgList{cfgReader_};
                    }
                }();
                if (cfgReader_) {
                    if (type_ == NodeListType::Copy && i > 0)
                        cfgReader_->load(nodeList.asList().node(0));
                    cfgReader_->load(treeNode);
                }
                maybeOptValue(nodeList_).emplace_back(std::move(cfg));
            } catch (const LoadingError& e) {
                throw ConfigError{"Node list '" + name_ + "': " + e.what(), treeNode.position()};
            }
        }
    }

    bool hasValue() const override
    {
        if constexpr (is_optional_v<TCfgList>)
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
        return "Node list '" + name_ + "'";
    }

private:
    std::string name_;
    TCfgList& nodeList_;
    bool hasValue_ = false;
    StreamPosition position_;
    NodeListType type_;
    ConfigReaderPtr cfgReader_;
};

}