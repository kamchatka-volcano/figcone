#pragma once
#include "nameformat.h"
#include "detail/inode.h"
#include "detail/iparam.h"
#include "detail/ivalidator.h"
#include "detail/iconfigreader.h"
#include "detail/nameconverter.h"
#include "detail/loadingerror.h"
#include "detail/figcone_json_import.h"
#include "detail/figcone_yaml_import.h"
#include "detail/figcone_toml_import.h"
#include "detail/figcone_ini_import.h"
#include "detail/figcone_xml_import.h"
#include <figcone_tree/iparser.h>
#include <figcone_tree/tree.h>
#include <map>
#include <vector>
#include <memory>
#include <filesystem>

namespace figcone {

template<NameFormat nameFormat = NameFormat::Original>
class ConfigReader : public detail::IConfigReader {
public:
    template<typename TCfg>
    TCfg readFile(const std::filesystem::path& configFile, IParser& parser)
    {
        auto cfg = TCfg{{makePtr()}};
        auto configStream = std::ifstream{configFile};
        read(configStream, parser);
        resetConfigReader(cfg);
        return cfg;
    }

    template<typename TCfg>
    TCfg read(const std::string& configContent, IParser& parser)
    {
        auto cfg = TCfg{{makePtr()}};
        auto configStream = std::stringstream{configContent};
        read(configStream, parser);
        resetConfigReader(cfg);
        return cfg;
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

private:
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

    detail::ConfigReaderPtr makeNestedReader(const std::string& name) override
    {
        nestedReaders_.emplace(name, std::make_unique<ConfigReader<nameFormat>>());
        return nestedReaders_[name]->makePtr();
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

    void swapContents(detail::ConfigReaderPtr otherReader) override
    {
        auto& reader = static_cast<ConfigReader<nameFormat>&>(*otherReader);
        std::swap(nodes_, reader.nodes_);
        std::swap(params_, reader.params_);
        std::swap(nestedReaders_, reader.nestedReaders_);
        std::swap(validators_, reader.validators_);
    }

private:
    std::map<std::string, std::unique_ptr<detail::INode>> nodes_;
    std::map<std::string, std::unique_ptr<detail::IParam>> params_;
    std::map<std::string, std::unique_ptr<ConfigReader<nameFormat>>> nestedReaders_;
    std::vector<std::unique_ptr<detail::IValidator>> validators_;
};

}