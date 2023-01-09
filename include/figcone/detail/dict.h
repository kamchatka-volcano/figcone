#pragma once
#include "inode.h"
#include "param.h"
#include "utils.h"
#include "external/sfun/type_traits.h"
#include <figcone_tree/tree.h>
#include <map>
#include <string>
#include <type_traits>

namespace figcone::detail {

template<typename TMap>
class Dict : public INode {
public:
    explicit Dict(std::string name, TMap& dictMap)
        : name_{std::move(name)}
        , dictMap_{dictMap}
    {
        static_assert(
                sfun::is_associative_container_v<sfun::remove_optional_t<TMap>>,
                "Dictionary field must be an associative container or an associative container placed in "
                "std::optional");
        static_assert(
                std::is_same_v<typename sfun::remove_optional_t<TMap>::key_type, std::string>,
                "Dictionary associative container's key type must be std::string");
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
            throw ConfigError{"Dictionary '" + name_ + "': config node can't be a list.", node.position()};
        if constexpr (sfun::is_optional_v<TMap>)
            dictMap_.emplace();
        maybeOptValue(dictMap_).clear();

        for (const auto& [paramName, paramValueStr] : node.asItem().params()) {
            auto paramValue =
                    convertFromString<typename sfun::remove_optional_t<TMap>::mapped_type>(paramValueStr.value());
            if (!paramValue)
                throw ConfigError{
                        "Couldn't set dict element'" + name_ + "' value from '" + paramValueStr.value() + "'",
                        position_};

            maybeOptValue(dictMap_).emplace(paramName, *paramValue);
        }
    }

    bool hasValue() const override
    {
        if constexpr (sfun::is_optional_v<TMap>)
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
        return "Dictionary '" + name_ + "'";
    }

private:
    std::string name_;
    TMap& dictMap_;
    bool hasValue_ = false;
    StreamPosition position_;
};

} //namespace figcone::detail