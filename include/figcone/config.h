#pragma once
#include "nameformat.h"
#include "detail/configmacros.h"
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
#include "detail/figcone_yaml_import.h"
#include "detail/figcone_toml_import.h"
#include "detail/figcone_ini_import.h"
#include "detail/figcone_xml_import.h"
#include "detail/nameof_import.h"
#include "detail/utils.h"
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
class IParam;
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
        readJson(configStream);
    }

    void readJson(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        readJson(configStream);
    }

    void readJson(std::istream& configStream)
    {
        auto parser = figcone::json::Parser{};
        read(configStream, parser);
    }
#endif

#ifdef FIGCONE_YAML_AVAILABLE
    void readYamlFile(const std::filesystem::path& configFile)
    {
        auto configStream = std::ifstream{configFile};
        readYaml(configStream);
    }

    void readYaml(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        readYaml(configStream);
    }

    void readYaml(std::istream& configStream)
    {
        auto parser = figcone::yaml::Parser{};
        read(configStream, parser);
    }
#endif

#ifdef FIGCONE_TOML_AVAILABLE
    void readTomlFile(const std::filesystem::path& configFile)
    {
        auto configStream = std::ifstream{configFile};
        readToml(configStream);
    }

    void readToml(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        readToml(configStream);
    }

    void readToml(std::istream& configStream)
    {
        auto parser = figcone::toml::Parser{};
        read(configStream, parser);
    }
#endif

#ifdef FIGCONE_INI_AVAILABLE
    void readIniFile(const std::filesystem::path& configFile)
    {
        auto configStream = std::ifstream{configFile};
        readIni(configStream);
    }

    void readIni(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        readIni(configStream);
    }

    void readIni(std::istream& configStream)
    {
        auto parser = figcone::ini::Parser{};
        read(configStream, parser);
    }
#endif

#ifdef FIGCONE_XML_AVAILABLE
    void readXmlFile(const std::filesystem::path& configFile)
    {
        auto configStream = std::ifstream{configFile};
        readXml(configStream);
    }

    void readXml(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        readXml(configStream);
    }

    void readXml(std::istream& configStream)
    {
        auto parser = figcone::xml::Parser{};
        read(configStream, parser);
    }
#endif

    static constexpr NameFormat format()
    {
        return nameFormat;
    }

protected:
    template<auto member>
    auto node(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return node<member>(ptr, memberName);
    }

    template<auto member>
    auto dict(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return dict<member>(ptr, memberName);
    }

    template<auto member>
    auto nodeList(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return nodeList<member>(ptr, memberName);
    }

    template<auto member>
    auto copyNodeList(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return copyNodeList<member>(ptr, memberName);
    }

    template<auto member>
    auto param(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return param<member>(ptr, memberName);
    }

    template<auto member>
    auto paramList(const std::string& memberName)
    {
        auto ptr = decltype(member){};
        return paramList<member>(ptr, memberName);
    }

#ifdef FIGCONE_NAMEOF_AVAILABLE
    template<auto member>
    auto node()
    {
        return node<member>(std::string{nameof::nameof_member<member>()});
    }

    template<auto member>
    auto dict()
    {
        return dict<member>(std::string{nameof::nameof_member<member>()});
    }

    template<auto member>
    auto nodeList()
    {
        return nodeList<member>(std::string{nameof::nameof_member<member>()});
    }

    template<auto member>
    auto copyNodeList()
    {
        return copyNodeList<member>(std::string{nameof::nameof_member<member>()});
    }

    template <auto member>
    auto param()
    {
        return param<member>(std::string{nameof::nameof_member<member>()});
    }

    template <auto member>
    auto paramList()
    {
        return paramList<member>(std::string{nameof::nameof_member<member>()});
    }
#endif

private:
    template <auto member, typename T, typename TCfg>
    auto node(T TCfg::*, const std::string& memberName)
    {
        static_assert(TCfg::format() == T::format(),
                      "Node's config type must have the same name format as its parent.");
        auto cfg = static_cast<TCfg*>(this);
        return detail::NodeCreator<T>{*this, memberName, cfg->*member};
    }

    template <auto member, typename TMap, typename TCfg>
    auto dict(TMap TCfg::*, const std::string& memberName)
    {
         static_assert(detail::is_associative_container_v<detail::maybe_opt_t<TMap>>,
              "Dictionary field must be an associative container or an associative container placed in std::optional");
        static_assert(std::is_same_v<typename detail::maybe_opt_t<TMap>::key_type, std::string>,
                     "Dictionary associative container's key type must be std::string");

        auto cfg = static_cast<TCfg*>(this);
        return detail::DictCreator<TMap>{*this, memberName, cfg->*member};
    }

    template <auto member, typename TCfgList, typename TCfg>
    auto nodeList(TCfgList TCfg::*, const std::string& memberName)
    {
        static_assert(detail::is_sequence_container_v<detail::maybe_opt_t<TCfgList>>,
              "Node list field must be a sequence container or a sequence container placed in std::optional");
        static_assert(TCfg::format() == detail::maybe_opt_t<TCfgList>::value_type::format(),
              "Node's config type must have the same name format as its parent.");
        auto cfg = static_cast<TCfg*>(this);
        return detail::NodeListCreator<TCfgList>{*this, memberName, cfg->*member};
    }

    template <auto member, typename TCfgList, typename TCfg>
    auto copyNodeList(TCfgList TCfg::*, const std::string& memberName)
    {
        static_assert(detail::is_sequence_container_v<detail::maybe_opt_t<TCfgList>>,
              "Node list field must be a sequence container or a sequence container placed in std::optional");
        static_assert(TCfg::format() == detail::maybe_opt_t<TCfgList>::value_type::format(),
              "Node's config type must have the same name format as its parent.");
        auto cfg = static_cast<TCfg*>(this);
        return detail::NodeListCreator<TCfgList>{*this, memberName, cfg->*member, detail::NodeListType::Copy};
    }

    template <auto member, typename T, typename TCfg>
    auto param(T TCfg::*, const std::string& memberName)
    {
        auto cfg = static_cast<TCfg*>(this);
        return detail::ParamCreator<T>{*this, memberName, cfg->*member};
    }

    template <auto member, typename T, typename TCfg>
    auto paramList(T TCfg::*, const std::string& memberName)
    {
        static_assert(detail::is_sequence_container_v<detail::maybe_opt_t<T>>,
                      "Param list field must be a sequence container or a sequence container placed in std::optional");

        auto cfg = static_cast<TCfg*>(this);
        return detail::ParamListCreator<T>{*this, memberName, cfg->*member};
    }

    void read(std::istream& configStream, IParser& parser)
    {
        auto tree = parser.parse(configStream);
        try {
            load(tree);
        }
        catch (const detail::LoadingError& e) {
            throw ConfigError{std::string{"Root node: "} + e.what(), tree.position()};
        }
    }

    void load(const TreeNode& treeNode) override
    {
        for (const auto& [nodeName, node] : treeNode.asItem().nodes()) {
            if (!nodes_.count(nodeName))
                throw ConfigError{"Unknown node '" + nodeName + "'", node.position()};
            try {
                nodes_.at(nodeName)->load(node);
            }
            catch (const detail::LoadingError& e) {
                throw ConfigError{"Node '" + nodeName + "': " + e.what(), node.position()};
            }
        }

        for (const auto&[paramName, param] : treeNode.asItem().params()) {
            if (!params_.count(paramName))
                throw ConfigError{"Unknown param '" + paramName + "'", param.position()};
            params_.at(paramName)->load(param);
        }

        checkLoadingResult();
    }

    void addNode(const std::string& name, std::unique_ptr<detail::INode> node) override
    {
        nodes_.emplace(detail::NameConverter<nameFormat>::name(name), std::move(node));
    }

    void addParam(const std::string& name, std::unique_ptr<detail::IParam> param) override
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
    std::map<std::string, std::unique_ptr<detail::INode>> nodes_;
    std::map<std::string, std::unique_ptr<detail::IParam>> params_;
    std::vector<std::unique_ptr<detail::IValidator>> validators_;
};

}
