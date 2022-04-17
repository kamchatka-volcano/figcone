#pragma once
#include "configmacros.h"
#include "nameformat.h"
#include "detail/iconfig.h"
#include "detail/inode.h"
#include "detail/ivalidator.h"
#include "detail/dict.h"
#include "detail/paramcreator.h"
#include "detail/nodelistcreator.h"
#include "detail/paramlistcreator.h"
#include "detail/dictcreator.h"
#include "detail/nameconverter.h"
#include "detail/loadingerror.h"
#include "detail/figcone_json_import.h"
#include <figcone_tree/iparser.h>
#include <figcone_tree/tree.h>
#include <vector>
#include <memory>
#include <map>
#include <string>
#include <optional>
#include <filesystem>
#include <fstream>

namespace figcone {
namespace detail {
class IConfigParam;
}

template<NameFormat nameFormat = NameFormat::Original>
class Config : public detail::IConfig {
public:
    void readFile(const std::filesystem::path& configFile, IParser& parser)
    {
        auto configStream = std::ifstream{configFile};
        read(configStream, parser);
    }

    void read(const std::string& configContent, IParser& parser)
    {
        auto configStream = std::stringstream{configContent};
        read(configStream, parser);
    }

#ifdef FIGCONE_JSON_AVAILABLE
    void readJsonFile(const std::filesystem::path& configFile)
    {
        auto configStream = std::ifstream{configFile};
        auto parser = figcone::json::Parser{};
        read(configStream, parser);
    }

    void readJson(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        auto parser = figcone::json::Parser{};
        read(configStream, parser);
    }
#endif

    static constexpr NameFormat format()
    {
        return nameFormat;
    }

protected:
    template<typename TCfg, typename T>
    auto node(T TCfg::* member, const std::string& name)
    {
        static_assert(TCfg::format() == T::format(),
                      "ConfigNode's config type must have the same name format as its parent.");
        return detail::ConfigNodeCreator<T>{*this, name, static_cast<TCfg*>(this)->*member};
    }

    template<typename TCfg>
    auto dict(std::map<std::string, std::string> TCfg::* member, const std::string& name)
    {
        return detail::ConfigDictCreator{*this, name, static_cast<TCfg*>(this)->*member};
    }

    template<typename TCfg, typename T>
    auto nodeList(std::vector<T> TCfg::* member, const std::string& name)
    {
        static_assert(TCfg::format() == T::format(),
                      "ConfigNode's config type must have the same name format as its parent.");
        return detail::ConfigNodeListCreator<T>{*this, name, static_cast<TCfg*>(this)->*member};
    }

    template<typename TCfg, typename T>
    auto param(T TCfg::* member, const std::string& name)
    {
        return detail::ConfigParamCreator<T>{*this, name, static_cast<TCfg*>(this)->*member};
    }

    template<typename TCfg, typename T>
    auto paramList(std::vector<T> TCfg::* member, const std::string& name)
    {
        return detail::ConfigParamListCreator<T>{*this, name, static_cast<TCfg*>(this)->*member};
    }

private:
    void read(std::istream& configStream, IParser& parser)
    {
        auto tree = parser.parse(configStream);
        try {
            load(tree);
        }
        catch (const detail::LoadingError& e) {
            throw ConfigError{tree.position(), std::string{"Root node: "} + e.what()};
        }
    }

    void load(const figcone::TreeNode& treeNode) override
    {
        for (const auto& [nodeName, node] : treeNode.asItem().nodes()) {
            if (!nodes_.count(nodeName))
                throw ConfigError{node.position(), "Unknown node '" + nodeName + "'"};
            try {
                nodes_.at(nodeName)->load(node);
            }
            catch (const detail::LoadingError& e) {
                throw ConfigError{node.position(), "Error in node '" + nodeName + "': " + e.what()};
            }
        }

        for (const auto&[paramName, param] : treeNode.asItem().params()) {
            if (!params_.count(paramName))
                throw ConfigError{param.position(), "Unknown param '" + paramName + "'"};
            params_.at(paramName)->load(param);
        }

        checkLoadingResult();
    }

    void addNode(const std::string& name, std::unique_ptr<detail::IConfigNode> node) override
    {
        nodes_.emplace(detail::NameConverter<nameFormat>::name(name), std::move(node));
    }

    void addParam(const std::string& name, std::unique_ptr<detail::IConfigParam> param) override
    {
        params_.emplace(detail::NameConverter<nameFormat>::name(name), std::move(param));
    }

    void addValidator(std::unique_ptr<detail::IValidator> validator) override
    {
        validators_.emplace_back((std::move(validator)));
    }

    void checkLoadingResult()
    {
        for (const auto&[name, param]: params_)
            if (!param->hasValue())
                throw detail::LoadingError{"Parameter '" + name + "' is missing."};
        for (const auto&[name, node]: nodes_)
            if (!node->hasValue())
                throw detail::LoadingError{"Node '" + name + "' is missing."};

        for (const auto& validator: validators_)
            validator->validate();
    }

private:
    std::map<std::string, std::unique_ptr<detail::IConfigNode>> nodes_;
    std::map<std::string, std::unique_ptr<detail::IConfigParam>> params_;
    std::vector<std::unique_ptr<detail::IValidator>> validators_;
};

}
