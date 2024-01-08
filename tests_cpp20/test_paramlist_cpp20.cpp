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

struct CfgInt {
    std::list<int> testIntList;
};

struct CfgOptInt {
    std::optional<std::deque<int>> testIntList;
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

struct ValidatedCfg {
    std::vector<std::string> testStrList;

    using traits = figcone::FieldTraits<figcone::ValidatedField<&ValidatedCfg::testStrList, NotEmpty>>;
};

struct ValidatedOptionalParamListCfg {
    std::optional<std::vector<std::string>> testStrList;

    using traits = figcone::FieldTraits<figcone::ValidatedField<&ValidatedOptionalParamListCfg::testStrList, NotEmpty>>;
};

struct CfgStr {
    std::vector<std::string> testStrList;
    std::vector<int> testIntList = std::vector{1, 2, 3};

    using traits = figcone::FieldTraits< //
            figcone::OptionalField<&CfgStr::testIntList>>;
};

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

TEST(StaticReflTestParamList, Basic)
{
    ///testIntList = [1, 2, 3]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParamList("testIntList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<CfgInt>("", parser);

    ASSERT_EQ(cfg.testIntList.size(), 3);
    EXPECT_EQ(*cfg.testIntList.begin(), 1);
    EXPECT_EQ(*std::next(cfg.testIntList.begin(), 1), 2);
    EXPECT_EQ(*std::next(cfg.testIntList.begin(), 2), 3);
}

TEST(StaticReflTestParamList, BasicOptional)
{
    ///testIntList = [1, 2, 3]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParamList("testIntList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<CfgOptInt>("", parser);

    ASSERT_TRUE(cfg.testIntList);
    ASSERT_EQ(cfg.testIntList->size(), 3);
    EXPECT_EQ(cfg.testIntList->at(0), 1);
    EXPECT_EQ(cfg.testIntList->at(1), 2);
    EXPECT_EQ(cfg.testIntList->at(2), 3);
}

TEST(StaticReflTestParamList, BasicMissingOptional)
{
    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<CfgOptInt>("", parser);

    ASSERT_FALSE(cfg.testIntList);
}

TEST(StaticReflTestParamList, ValidationSuccess)
{
    ///testStrList = [1, 2, 3]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParamList("testStrList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<ValidatedCfg>("", parser);

    ASSERT_EQ(cfg.testStrList.size(), 3);
    EXPECT_EQ(cfg.testStrList.at(0), "1");
    EXPECT_EQ(cfg.testStrList.at(1), "2");
    EXPECT_EQ(cfg.testStrList.at(2), "3");
}

TEST(StaticReflTestParamList, ValidationSuccessOptionalParamList)
{
    ///testStrList = [1, 2, 3]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParamList("testStrList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<ValidatedOptionalParamListCfg>("", parser);

    ASSERT_EQ(cfg.testStrList.has_value(), true);
    ASSERT_EQ(cfg.testStrList->size(), 3);
    EXPECT_EQ(cfg.testStrList->at(0), "1");
    EXPECT_EQ(cfg.testStrList->at(1), "2");
    EXPECT_EQ(cfg.testStrList->at(2), "3");
}

TEST(StaticReflTestParamList, ValidationError)
{
    ///testStrList = []
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParamList("testStrList", std::vector<std::string>{}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};

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

TEST(StaticReflTestParamList, ValidationErrorOptionalParamList)
{
    ///testStrList = []
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParamList("testStrList", std::vector<std::string>{}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};

    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedOptionalParamListCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Parameter list 'testStrList': a list can't be empty");
            });
}

TEST(StaticReflTestParamList, MissingParamListError)
{
    ///testIntList = [1, 2]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParamList("testIntList", std::vector<std::string>{"1", "2"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};

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

TEST(StaticReflTestParamList, ParamNotListError)
{
    ///testIntList = 1
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("testIntList", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};

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

TEST(StaticReflTestParamList, DefaultValue)
{
    ///testStrList = []
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParamList("testStrList", std::vector<std::string>{}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<CfgStr>("", parser);

    ASSERT_EQ(cfg.testStrList.size(), 0);
    ASSERT_EQ(cfg.testIntList.size(), 3);
    EXPECT_EQ(cfg.testIntList.at(0), 1);
    EXPECT_EQ(cfg.testIntList.at(1), 2);
    EXPECT_EQ(cfg.testIntList.at(2), 3);
}

} //namespace test_paramlist
