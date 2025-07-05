#ifndef FIGCONE_NODELIST_H
#define FIGCONE_NODELIST_H

#include "configreaderaccess.h"
#include "inode.h"
#include "loadingerror.h"
#include "utils.h"
#include "external/eel/type_traits.h"
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <memory>
#include <type_traits>
#include <vector>

namespace figcone::detail {

enum class NodeListType {
    Normal,
    Copy
};

template<typename TCfgList>
class NodeList : public detail::INode {
public:
    NodeList(std::string name, TCfgList& nodeList, ConfigReaderPtr cfgReader, NodeListType type = NodeListType::Normal)
        : name_{std::move(name)}
        , nodeList_{nodeList}
        , type_{type}
        , cfgReader_{cfgReader}
    {
        static_assert(
                eel::is_dynamic_sequence_container_v<eel::remove_optional_t<TCfgList>>,
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
        nodeList_ = TCfgList{};
        if (!nodeList.isList())
            throw ConfigError{"Node list '" + name_ + "': config node must be a list.", nodeList.position()};
        if constexpr (eel::is_optional<TCfgList>::value)
            nodeList_.emplace();

        maybeOptValue(nodeList_).clear();
        for (auto i = 0; i < nodeList.asList().size(); ++i) {
            const auto& treeNode = nodeList.asList().at(i);
            try {
                using Cfg = typename eel::remove_optional_t<TCfgList>::value_type;
                if constexpr (std::is_base_of_v<figcone::Config, Cfg>) {
                    if constexpr (!std::is_aggregate_v<Cfg>)
                        static_assert(
                                std::is_constructible_v<Cfg, detail::ConfigReaderPtr>,
                                "Non aggregate config objects must inherit figcone::Config constructors with 'using "
                                "Config::Config;'");

                    auto cfg = Cfg{cfgReader_};
                    if (cfgReader_) {
                        if (type_ == NodeListType::Copy && i > 0)
                            ConfigReaderAccess{cfgReader_}.load<Cfg>(nodeList.asList().at(0));
                        ConfigReaderAccess{cfgReader_}.load<Cfg>(treeNode);
                    }
                    maybeOptValue(nodeList_).emplace_back(std::move(cfg));
                }
                else {
                    auto cfg = Cfg{};
                    if (cfgReader_) {
                        ConfigReaderAccess{cfgReader_}.loadStructure<Cfg>(cfg);

                        if (type_ == NodeListType::Copy && i > 0)
                            ConfigReaderAccess{cfgReader_}.load<Cfg>(nodeList.asList().at(0));
                        ConfigReaderAccess{cfgReader_}.load<Cfg>(treeNode);
                    }
                    maybeOptValue(nodeList_).emplace_back(std::move(cfg));
                }
            }
            catch (const LoadingError& e) {
                throw ConfigError{"Node list '" + name_ + "': " + e.what(), treeNode.position()};
            }
        }
    }

    bool hasValue() const override
    {
        if constexpr (eel::is_optional_v<TCfgList>)
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

} //namespace figcone::detail

#endif //FIGCONE_NODELIST_H