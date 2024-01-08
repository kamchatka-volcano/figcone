#pragma once
#include "errors.h"
#include "nameformat.h"
#include "postprocessor.h"
#include "unregisteredfieldhandler.h"
#include "detail/configreaderptr.h"
#include "detail/external/sfun/path.h"
#include "detail/external/sfun/type_traits.h"
#include "detail/figcone_ini_import.h"
#include "detail/figcone_json_import.h"
#include "detail/figcone_shoal_import.h"
#include "detail/figcone_toml_import.h"
#include "detail/figcone_xml_import.h"
#include "detail/figcone_yaml_import.h"
#include "detail/inode.h"
#include "detail/iparam.h"
#include "detail/ivalidator.h"
#include "detail/loadingerror.h"
#include "detail/nameutils.h"
#include "detail/unregisteredfieldutils.h"
#include <figcone_tree/iparser.h>
#include <figcone_tree/tree.h>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <type_traits>
#include <vector>

namespace figcone {

namespace detail {
template<typename TConfigReaderPtr>
class ConfigReaderAccess;
}

enum class RootType {
    SingleNode,
    NodeList
};

class ConfigReader {

public:
    explicit ConfigReader(NameFormat nameFormat = NameFormat::Original)
        : nameFormat_{nameFormat}
    {
    }

    template<typename TCfg, RootType rootType = RootType::SingleNode>
    auto readFile(const std::filesystem::path& configFile, IParser& parser)
            -> std::conditional_t<rootType == RootType::SingleNode, TCfg, std::vector<TCfg>>
    {
        if (!std::filesystem::exists(configFile))
            throw ConfigError{"Config file " + sfun::path_string(configFile) + " doesn't exist"};

        if (!std::filesystem::is_regular_file(configFile))
            throw ConfigError{
                    "Can't open config file " + sfun::path_string(configFile) + " which is not a regular file"};

        auto configStream = std::ifstream{configFile, std::ios_base::binary};
        if (!configStream.is_open())
            throw ConfigError{"Can't open config file " + sfun::path_string(configFile) + " for reading"};

        return read<TCfg, rootType>(configStream, parser);
    }

    template<typename TCfg, RootType rootType = RootType::SingleNode>
    auto read(const std::string& configContent, IParser& parser)
            -> std::conditional_t<rootType == RootType::SingleNode, TCfg, std::vector<TCfg>>
    {
        auto configStream = std::stringstream{configContent};
        return read<TCfg, rootType>(configStream, parser);
    }

#ifdef FIGCONE_JSON_AVAILABLE
    template<typename TCfg, RootType rootType = RootType::SingleNode>
    auto readJsonFile(const std::filesystem::path& configFile)
            -> std::conditional_t<rootType == RootType::SingleNode, TCfg, std::vector<TCfg>>
    {
        auto parser = figcone::json::Parser{};
        return readFile<TCfg, rootType>(configFile, parser);
    }

    template<typename TCfg, RootType rootType = RootType::SingleNode>
    auto readJson(const std::string& configContent)
            -> std::conditional_t<rootType == RootType::SingleNode, TCfg, std::vector<TCfg>>
    {
        auto parser = figcone::json::Parser{};
        return read<TCfg, rootType>(configContent, parser);
    }

    template<typename TCfg, RootType rootType = RootType::SingleNode>
    auto readJson(std::istream& configStream)
            -> std::conditional_t<rootType == RootType::SingleNode, TCfg, std::vector<TCfg>>
    {
        auto parser = figcone::json::Parser{};
        return read<TCfg, rootType>(configStream, parser);
    }

#endif

#ifdef FIGCONE_YAML_AVAILABLE
    template<typename TCfg, RootType rootType = RootType::SingleNode>
    auto readYamlFile(const std::filesystem::path& configFile)
            -> std::conditional_t<rootType == RootType::SingleNode, TCfg, std::vector<TCfg>>
    {
        auto parser = figcone::yaml::Parser{};
        return readFile<TCfg, rootType>(configFile, parser);
    }

    template<typename TCfg, RootType rootType = RootType::SingleNode>
    auto readYaml(const std::string& configContent)
            -> std::conditional_t<rootType == RootType::SingleNode, TCfg, std::vector<TCfg>>
    {
        auto parser = figcone::yaml::Parser{};
        return read<TCfg, rootType>(configContent, parser);
    }

    template<typename TCfg, RootType rootType = RootType::SingleNode>
    auto readYaml(std::istream& configStream)
            -> std::conditional_t<rootType == RootType::SingleNode, TCfg, std::vector<TCfg>>
    {
        auto parser = figcone::yaml::Parser{};
        return read<TCfg, rootType>(configStream, parser);
    }
#endif

#ifdef FIGCONE_TOML_AVAILABLE
    template<typename TCfg>
    TCfg readTomlFile(const std::filesystem::path& configFile)
    {
        auto parser = figcone::toml::Parser{};
        return readFile<TCfg>(configFile, parser);
    }
    template<typename TCfg>
    TCfg readToml(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        return readToml<TCfg>(configStream);
    }
    template<typename TCfg>
    TCfg readToml(std::istream& configStream)
    {
        auto parser = figcone::toml::Parser{};
        return read<TCfg>(configStream, parser);
    }
#endif

#ifdef FIGCONE_INI_AVAILABLE
    template<typename TCfg>
    TCfg readIniFile(const std::filesystem::path& configFile)
    {
        auto parser = figcone::ini::Parser{};
        return readFile<TCfg>(configFile, parser);
    }
    template<typename TCfg>
    TCfg readIni(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        return readIni<TCfg>(configStream);
    }
    template<typename TCfg>
    TCfg readIni(std::istream& configStream)
    {
        auto parser = figcone::ini::Parser{};
        return read<TCfg>(configStream, parser);
    }
#endif

#ifdef FIGCONE_XML_AVAILABLE
    template<typename TCfg>
    TCfg readXmlFile(const std::filesystem::path& configFile)
    {
        auto parser = figcone::xml::Parser{};
        return readFile<TCfg>(configFile, parser);
    }
    template<typename TCfg>
    TCfg readXml(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        return readXml<TCfg>(configStream);
    }
    template<typename TCfg>
    TCfg readXml(std::istream& configStream)
    {
        auto parser = figcone::xml::Parser{};
        return read<TCfg>(configStream, parser);
    }
#endif

#ifdef FIGCONE_SHOAL_AVAILABLE
    template<typename TCfg>
    TCfg readShoalFile(const std::filesystem::path& configFile)
    {
        auto parser = figcone::shoal::Parser{};
        return readFile<TCfg>(configFile, parser);
    }
    template<typename TCfg>
    TCfg readShoal(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        return readShoal<TCfg>(configStream);
    }
    template<typename TCfg>
    TCfg readShoal(std::istream& configStream)
    {
        auto parser = figcone::shoal::Parser{};
        return read<TCfg>(configStream, parser);
    }
#endif

private:
    void addNode(const std::string& name, std::unique_ptr<detail::INode> node)
    {
        nodes_.emplace(detail::convertName(nameFormat_, name), std::move(node));
    }

    void addParam(const std::string& name, std::unique_ptr<detail::IParam> param)
    {
        params_.emplace(detail::convertName(nameFormat_, name), std::move(param));
    }

    void addValidator(std::unique_ptr<detail::IValidator> validator)
    {
        validators_.emplace_back((std::move(validator)));
    }

    template<typename TConfig>
    void load(const TreeNode& treeNode)
    {
        for (const auto& nodeName : treeNode.asItem().nodeNames()) {
            const auto& node = treeNode.asItem().node(nodeName);
            if (!nodes_.count(nodeName)) {
                detail::handleUnregisteredField<TConfig>(FieldType::Node, nodeName, node.position());
                continue;
            }

            try {
                nodes_.at(nodeName)->load(node);
            }
            catch (const detail::LoadingError& e) {
                throw ConfigError{"Node '" + nodeName + "': " + e.what(), node.position()};
            }
        }

        for (const auto& paramName : treeNode.asItem().paramNames()) {
            const auto& param = treeNode.asItem().param(paramName);
            if (!params_.count(paramName)) {
                detail::handleUnregisteredField<TConfig>(FieldType::Param, paramName, param.position());
                continue;
            }
            params_.at(paramName)->load(param);
        }

        checkLoadingResult();
    }

    void checkLoadingResult()
    {
        for (const auto& [name, param] : params_)
            if (!param->hasValue())
                throw detail::LoadingError{"Parameter '" + name + "' is missing."};
        for (const auto& [name, node] : nodes_)
            if (!node->hasValue())
                throw detail::LoadingError{"Node '" + name + "' is missing."};

        for (const auto& validator : validators_)
            validator->validate();
    }

    detail::ConfigReaderPtr makeNestedReader(const std::string& name)
    {
        nestedReaders_.emplace(name, std::make_unique<ConfigReader>(nameFormat_));
        return nestedReaders_[name]->makePtr();
    }

    template<typename TCfg, RootType rootType = RootType::SingleNode>
    auto read(std::istream& configStream, IParser& parser)
            -> std::conditional_t<rootType == RootType::SingleNode, TCfg, std::vector<TCfg>>
    {
        auto tree = parser.parse(configStream);
        auto result = std::vector<TCfg>{};
        if (tree.root().isList()) {
            for (auto i = 0; i < tree.root().asList().size(); ++i)
                result.emplace_back(readConfig<TCfg>(tree.root().asList().at(i)));
        }
        else
            result.emplace_back(readConfig<TCfg>(tree.root()));

        if constexpr (rootType == RootType::SingleNode) {
            if (result.size() != 1)
                throw ConfigError{"Expected a single element root of the document, use 'readList*' methods instead"};

            return std::move(result.at(0));
        }
        else
            return result;
    }

    void clear()
    {
        nodes_.clear();
        params_.clear();
        nestedReaders_.clear();
        validators_.clear();
    }

    detail::ConfigReaderPtr makePtr()
    {
        return this;
    }

    template<typename TCfg>
    void resetConfigReader(TCfg& cfg)
    {
        cfg.cfgReader_ = detail::ConfigReaderPtr{};
    }

    template<typename TCfg>
    TCfg readConfig(const figcone::TreeNode& root)
    {
        if constexpr (!std::is_aggregate_v<TCfg>)
            static_assert(
                    std::is_constructible_v<TCfg, detail::ConfigReaderPtr>,
                    "Non aggregate config objects must inherit figcone::Config constructors with 'using "
                    "Config::Config;'");
        clear();
        auto cfg = TCfg{makePtr()};
        try {
            load<TCfg>(root);
        }
        catch (const detail::LoadingError& e) {
            throw ConfigError{std::string{"Root node: "} + e.what(), root.position()};
        }
        try {
            PostProcessor<TCfg>{}(cfg);
        }
        catch (const ValidationError& e) {
            throw ConfigError{std::string{"Config is invalid: "} + e.what()};
        }
        resetConfigReader(cfg);
        return cfg;
    }

private:
    template<typename TConfigReaderPtr>
    friend class detail::ConfigReaderAccess;

private:
    std::map<std::string, std::unique_ptr<detail::INode>> nodes_;
    std::map<std::string, std::unique_ptr<detail::IParam>> params_;
    std::map<std::string, std::unique_ptr<ConfigReader>> nestedReaders_;
    std::vector<std::unique_ptr<detail::IValidator>> validators_;
    NameFormat nameFormat_;
};

} //namespace figcone