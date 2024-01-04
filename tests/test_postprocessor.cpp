#include "assert_exception.h"
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <optional>

namespace test_postprocessor {

using namespace figcone;

struct Cfg : public Config {
    FIGCONE_PARAM(name, figcone::optional<std::string>);
    FIGCONE_PARAM(surname, std::string);
    FIGCONE_PARAM(age, int);
};

} //namespace test_postprocessor

namespace figcone {
template<>
void PostProcessor<test_postprocessor::Cfg>::operator()(test_postprocessor::Cfg& cfg)
{
    if (!cfg.name.has_value())
        std::transform(
                cfg.surname.begin(),
                cfg.surname.end(),
                cfg.surname.begin(),
                [](const auto& ch)
                {
                    return sfun::toupper(ch);
                });
    if (!cfg.name.has_value() && cfg.age < 18)
        throw ValidationError{"Name field must be specified for underage persons"};
}
} //namespace figcone

namespace test_postprocessor {

class TreeProvider : public figcone::IParser {
public:
    TreeProvider(std::unique_ptr<figcone::TreeNode> tree)
        : tree_{std::move(tree)}
    {
    }

    figcone::Tree parse(std::istream&) override
    {
        return std::move(tree_);
    }

    std::unique_ptr<figcone::TreeNode> tree_;
};

TEST(TestPostProcessor, NonModifyingPostProcessor)
{
    ///
    /// name = John
    /// surname = Doe
    /// age = 42
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("name", "John", {1, 1});
    tree->asItem().addParam("surname", "Doe", {2, 1});
    tree->asItem().addParam("age", "42", {3, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<Cfg>("", parser);

    ASSERT_EQ(cfg.name, "John");
    ASSERT_EQ(cfg.surname, "Doe");
    ASSERT_EQ(cfg.age, 42);
}

TEST(TestPostProcessor, PostProcessorModifiesCfg)
{
    ///
    /// name = John
    /// surname = Doe
    /// age = 42
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("surname", "Doe", {2, 1});
    tree->asItem().addParam("age", "42", {3, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<Cfg>("", parser);

    ASSERT_EQ(cfg.name.has_value(), false);
    ASSERT_EQ(cfg.surname, "DOE");
    ASSERT_EQ(cfg.age, 42);
}

TEST(TestPostProcessor, PostProcessorThrowsError)
{
    ///
    /// name = John
    /// surname = Doe
    /// age = 42
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("surname", "Doe", {2, 1});
    tree->asItem().addParam("age", "16", {3, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};

    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<Cfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "Config is invalid: Name field must be specified for underage persons");
            });
}

TEST(TestPostProcessor, RootNodeListPostProcessorModifiesCfg)
{
    ///
    /// name = John
    /// surname = Doe
    /// age = 42
    /// ---
    /// surname = Doe
    /// age = 33

    auto tree = figcone::makeTreeRootList();
    auto& firstCfg = tree->asList().emplaceBack();
    firstCfg.asItem().addParam("name", "John", {1, 1});
    firstCfg.asItem().addParam("surname", "Doe", {2, 1});
    firstCfg.asItem().addParam("age", "42", {3, 1});
    auto& secondCfg = tree->asList().emplaceBack();
    secondCfg.asItem().addParam("surname", "Doe", {5, 1});
    secondCfg.asItem().addParam("age", "33", {6, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};

    auto cfgList = cfgReader.read<Cfg, figcone::RootType::NodeList>("", parser);
    ASSERT_EQ(cfgList.size(), 2);
    ASSERT_EQ(cfgList.at(0).name.has_value(), true);
    ASSERT_EQ(cfgList.at(0).name.value(), "John");
    ASSERT_EQ(cfgList.at(0).surname, "Doe");
    ASSERT_EQ(cfgList.at(0).age, 42);
    ASSERT_EQ(cfgList.at(1).name.has_value(), false);
    ASSERT_EQ(cfgList.at(1).surname, "DOE");
    ASSERT_EQ(cfgList.at(1).age, 33);
}

} //namespace test_postprocessor