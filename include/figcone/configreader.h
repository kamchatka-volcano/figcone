#pragma once
#include "errors.h"
#include "nameformat.h"
#include "postprocessor.h"
#include "detail/configreaderptr.h"
#include "detail/external/sfun/path.h"
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

class ConfigReader {
public:
    explicit ConfigReader(NameFormat nameFormat = NameFormat::Original)
        : nameFormat_{nameFormat}
    {
    }

    template<typename TCfg>
    TCfg readFile(const std::filesystem::path& configFile, IParser& parser)
    {
        if (!std::filesystem::exists(configFile))
            throw ConfigError{"Config file " + sfun::path_string(configFile) + " doesn't exist"};

        if (!std::filesystem::is_regular_file(configFile))
            throw ConfigError{
                    "Can't open config file " + sfun::path_string(configFile) + " which is not a regular file"};

        auto configStream = std::ifstream{configFile, std::ios_base::binary};
        if (!configStream.is_open())
            throw ConfigError{"Can't open config file " + sfun::path_string(configFile) + " for reading"};

        return read<TCfg>(configStream, parser);
    }

    template<typename TCfg>
    TCfg read(const std::string& configContent, IParser& parser)
    {
        auto configStream = std::stringstream{configContent};
        return read<TCfg>(configStream, parser);
    }

#ifdef FIGCONE_JSON_AVAILABLE
    template<typename TCfg>
    TCfg readJsonFile(const std::filesystem::path& configFile)
    {
        auto parser = figcone::json::Parser{};
        return readFile<TCfg>(configFile, parser);
    }
    template<typename TCfg>
    TCfg readJson(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        return readJson<TCfg>(configStream);
    }
    template<typename TCfg>
    TCfg readJson(std::istream& configStream)
    {
        auto parser = figcone::json::Parser{};
        return read<TCfg>(configStream, parser);
    }
#endif

#ifdef FIGCONE_YAML_AVAILABLE
    template<typename TCfg>
    TCfg readYamlFile(const std::filesystem::path& configFile)
    {
        auto parser = figcone::yaml::Parser{};
        return readFile<TCfg>(configFile, parser);
    }
    template<typename TCfg>
    TCfg readYaml(const std::string& configContent)
    {
        auto configStream = std::stringstream{configContent};
        return readYaml<TCfg>(configStream);
    }

    template<typename TCfg>
    TCfg readYaml(std::istream& configStream)
    {
        auto parser = figcone::yaml::Parser{};
        return read<TCfg>(configStream, parser);
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

    void load(const TreeNode& treeNode)
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

        for (const auto& [paramName, param] : treeNode.asItem().params()) {
            if (!params_.count(paramName))
                throw ConfigError{"Unknown param '" + paramName + "'", param.position()};
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

    template<typename TCfg>
    TCfg read(std::istream& configStream, IParser& parser)
    {
        clear();
        if constexpr (!std::is_aggregate_v<TCfg>)
            static_assert(
                    std::is_constructible_v<TCfg, detail::ConfigReaderPtr>,
                    "Non aggregate config objects must inherit figcone::Config constructors with 'using "
                    "Config::Config;'");
        auto cfg = TCfg{makePtr()};
        auto tree = parser.parse(configStream);
        try {
            load(tree);
        }
        catch (const detail::LoadingError& e) {
            throw ConfigError{std::string{"Root node: "} + e.what(), tree.position()};
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