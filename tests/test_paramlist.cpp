#include "assert_exception.h"
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <gtest/gtest.h>
#include <deque>
#include <list>
#include <optional>
#include <vector>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif

namespace test_paramlist {

struct CfgInt : public figcone::Config {
    FIGCONE_PARAMLIST(testIntList, std::list<int>);
};

struct CfgOptInt : public figcone::Config {
    FIGCONE_PARAMLIST(testIntList, figcone::optional<std::deque<int>>);
};

struct NotEmpty {
    template<typename T>
    void operator()(const T& value)
    {
        if (value.empty())
            throw figcone::ValidationError{"a list can't be empty"};
    }
};

struct HasNoEmptyElements {
    template<typename T>
    void operator()(const T& value)
    {
        if (std::find_if(
                    value.begin(),
                    value.end(),
                    [](const auto& str)
                    {
                        return str.empty();
                    }) != value.end())
            throw figcone::ValidationError{"a list element can't be empty"};
    }
};

struct ValidatedCfg : public figcone::Config {
    FIGCONE_PARAMLIST(testStrList, std::vector<std::string>)
            .ensure(
                    [](const std::vector<std::string>& value)
                    {
                        if (value.empty())
                            throw figcone::ValidationError{"a list can't be empty"};
                    });
};

struct ValidatedWithFunctorCfg : public figcone::Config {
    FIGCONE_PARAMLIST(testStrList, std::vector<std::string>).ensure<NotEmpty>().ensure<HasNoEmptyElements>();
};

#ifdef NAMEOF_AVAILABLE
struct CfgIntWithoutMacro : public figcone::Config {
    std::list<int> testIntList = paramList<&CfgIntWithoutMacro::testIntList>();
};
#else
struct CfgIntWithoutMacro : public figcone::Config {
    std::list<int> testIntList = paramList<&CfgIntWithoutMacro::testIntList>("testIntList");
};
#endif

struct CfgStr : public figcone::Config {
    FIGCONE_PARAMLIST(testStrList, std::vector<std::string>);
    FIGCONE_PARAMLIST(testIntList, std::vector<int>)({1, 2, 3});
};

class TreeProvider : public figcone::IParser {
public:
    TreeProvider(figcone::TreeNode tree)
        : tree_(std::move(tree))
    {
    }

    figcone::TreeNode parse(std::istream&) override
    {
        return std::move(tree_);
    }

    figcone::TreeNode tree_;
};

TEST(TestParamList, Basic)
{
    ///testIntList = [1, 2, 3]
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testIntList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<CfgInt>("", parser);

    ASSERT_EQ(cfg.testIntList.size(), 3);
    EXPECT_EQ(*cfg.testIntList.begin(), 1);
    EXPECT_EQ(*std::next(cfg.testIntList.begin(), 1), 2);
    EXPECT_EQ(*std::next(cfg.testIntList.begin(), 2), 3);
}

TEST(TestParamList, BasicOptional)
{
    ///testIntList = [1, 2, 3]
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testIntList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<CfgOptInt>("", parser);

    ASSERT_TRUE(cfg.testIntList);
    ASSERT_EQ(cfg.testIntList->size(), 3);
    EXPECT_EQ(cfg.testIntList->at(0), 1);
    EXPECT_EQ(cfg.testIntList->at(1), 2);
    EXPECT_EQ(cfg.testIntList->at(2), 3);
}

TEST(TestParamList, BasicMissingOptional)
{
    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<CfgOptInt>("", parser);

    ASSERT_FALSE(cfg.testIntList);
}

TEST(TestParamList, BasicWithoutMacro)
{
    ///testIntList = [1, 2, 3]
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testIntList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<CfgIntWithoutMacro>("", parser);

    ASSERT_EQ(cfg.testIntList.size(), 3);
    EXPECT_EQ(*cfg.testIntList.begin(), 1);
    EXPECT_EQ(*std::next(cfg.testIntList.begin(), 1), 2);
    EXPECT_EQ(*std::next(cfg.testIntList.begin(), 2), 3);
}

TEST(TestParamList, ValidationSuccess)
{
    ///testStrList = [1, 2, 3]
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<ValidatedCfg>("", parser);

    ASSERT_EQ(cfg.testStrList.size(), 3);
    EXPECT_EQ(cfg.testStrList.at(0), "1");
    EXPECT_EQ(cfg.testStrList.at(1), "2");
    EXPECT_EQ(cfg.testStrList.at(2), "3");
}

TEST(TestParamList, ValidationError)
{
    ///testStrList = []
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};

    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Parameter list 'testStrList': a list can't be empty");
            });
}

TEST(TestParamList, ValidationWithFunctorError)
{
    ///testStrList = []
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedWithFunctorCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Parameter list 'testStrList': a list can't be empty");
            });
}

TEST(TestParamList, ValidationWithFunctorError2)
{
    ///testStrList = ['hello', '']
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{"hello", ""}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};

    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedWithFunctorCfg>("testStrList = 'hello', ''", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Parameter list 'testStrList': a list element can't be empty");
            });
}

TEST(TestParamList, MissingParamListError)
{
    ///testIntList = [1, 2]
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testIntList", std::vector<std::string>{"1", "2"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};

    assert_exception<figcone::ConfigError>(
            [&]
            {
                auto cfg = cfgReader.read<CfgStr>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Root node: Parameter 'testStrList' is missing.");
            });
}

TEST(TestParamList, ParamNotListError)
{
    ///testIntList = 1
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testIntList", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};

    assert_exception<figcone::ConfigError>(
            [&]
            {
                auto cfg = cfgReader.read<CfgInt>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Parameter list 'testIntList': config parameter must be a list.");
            });
}

TEST(TestParamList, DefaultValue)
{
    ///testStrList = []
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<CfgStr>("", parser);

    ASSERT_EQ(cfg.testStrList.size(), 0);
    ASSERT_EQ(cfg.testIntList.size(), 3);
    EXPECT_EQ(cfg.testIntList.at(0), 1);
    EXPECT_EQ(cfg.testIntList.at(1), 2);
    EXPECT_EQ(cfg.testIntList.at(2), 3);
}

} //namespace test_paramlist
