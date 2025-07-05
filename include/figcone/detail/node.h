#ifndef FIGCONE_NODE_H
#define FIGCONE_NODE_H

#include "configreaderaccess.h"
#include "iconfigentity.h"
#include "inode.h"
#include "utils.h"
#include "external/eel/type_traits.h"
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <algorithm>
#include <sstream>
#include <string>

namespace figcone {
class Config;
}

namespace figcone::detail {

template<typename TCfg>
class Node : public INode {
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

        if constexpr (is_initialized_optional_v<TCfg> || eel::is_optional_v<TCfg>)
            cfg_.emplace();

        if constexpr (!std::is_base_of_v<figcone::Config, eel::remove_optional_t<TCfg>>) {
            ConfigReaderAccess{cfgReader_}.loadStructure<eel::remove_optional_t<TCfg>>(maybeOptValue(cfg_));
        }

        if (cfgReader_)
            ConfigReaderAccess{cfgReader_}.load<TCfg>(node);
    }

    bool hasValue() const override
    {
        if constexpr (is_initialized_optional_v<TCfg> || eel::is_optional_v<TCfg>)
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

} //namespace figcone::detail

#endif //FIGCONE_NODE_H