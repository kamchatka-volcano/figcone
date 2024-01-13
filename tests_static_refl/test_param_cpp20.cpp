#include "assert_exception.h"
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/stringconverter.h>
#include <figcone_tree/tree.h>
#include <gtest/gtest.h>
#include <filesystem>
#include <optional>

namespace test_param {

struct SingleParamCfg {
    int test;
};

struct IsPositive {
public:
    void operator()(int val)
    {
        if (val < 0)
            throw figcone::ValidationError{"value can't be negative"};
    }
};

struct ValidatedCfg {
    int test;

    using traits = figcone::FieldTraits<figcone::ValidatedField<&ValidatedCfg::test, IsPositive>>;
};

struct ValidatedOptionalParamCfg {
    std::optional<int> test;

    using traits = figcone::FieldTraits<figcone::ValidatedField<&ValidatedOptionalParamCfg::test, IsPositive>>;
};
//
//struct ValidatedWithFunctorOptionalParamCfg {
//    FIGCONE_PARAM(test, figcone::optional<int>).ensure<IsPositive>();
//};

struct StringParamCfg {
    std::string test;
    std::filesystem::path testFs = "default.txt";

    using traits = figcone::FieldTraits< //
            figcone::OptionalField<&StringParamCfg::testFs>>;
};

struct MultiParamCfg {
    int testInt;
    double testDouble = 9.0;
    std::optional<std::string> testString;
    std::optional<std::string> testString2;

    using traits = figcone::FieldTraits< //
            figcone::OptionalField<&MultiParamCfg::testDouble>>;
};

struct UserType {
    std::string value;
};
} //namespace test_param

template<>
struct figcone::StringConverter<test_param::UserType> {
    static std::optional<test_param::UserType> fromString(const std::string& data)
    {
        auto result = test_param::UserType{};
        result.value = data;
        return result;
    }
};

namespace test_param {
struct SingleUserTypeParamCfg {
    UserType test;
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

TEST(StaticReflTestParam, IntParam)
{
    ///test=1
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("test", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<SingleParamCfg>("", parser);

    EXPECT_EQ(cfg.test, 1);
}

TEST(StaticReflTestParam, StringParam)
{
    ///test=hello
    ///testFs='hello world.txt'
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("test", "hello", {1, 1});
    tree->asItem().addParam("testFs", "hello world.txt", {2, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<StringParamCfg>("", parser);

    EXPECT_EQ(cfg.test, "hello");
    EXPECT_EQ(cfg.testFs, "hello world.txt");
}

TEST(StaticReflTestParam, UserTypeParam)
{
    ///test='hello world'
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("test", "hello world", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<SingleUserTypeParamCfg>("", parser);

    EXPECT_EQ(cfg.test.value, "hello world");
}

TEST(StaticReflTestParam, EmptyStringParam)
{
    ///test=''
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("test", "", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<StringParamCfg>("", parser);

    EXPECT_EQ(cfg.test, "");
}
//
TEST(StaticReflTestParam, MultiParam)
{
    ///testInt=5
    ///testDouble=5.0
    ///testString='foo'
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("testInt", "5", {1, 1});
    tree->asItem().addParam("testDouble", "5.0", {2, 1});
    tree->asItem().addParam("testString", "foo", {3, 1});
    tree->asItem().addParam("testString2", "Hello world", {4, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<MultiParamCfg>("", parser);

    EXPECT_EQ(cfg.testInt, 5);
    EXPECT_EQ(cfg.testDouble, 5.0);
    ASSERT_TRUE(cfg.testString.has_value());
    EXPECT_EQ(*cfg.testString, "foo");
    ASSERT_TRUE(cfg.testString2.has_value());
    EXPECT_EQ(*cfg.testString2, "Hello world");
}

TEST(StaticReflTestParam, MultiParamUnspecifiedOptionals)
{
    ///testInt=5
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("testInt", "5", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<MultiParamCfg>("", parser);

    EXPECT_EQ(cfg.testInt, 5);
    EXPECT_EQ(cfg.testDouble, 9.0);
    ASSERT_FALSE(cfg.testString.has_value());
    ASSERT_FALSE(cfg.testString2.has_value());
}

TEST(StaticReflTestParam, ValidationSuccess)
{
    ///test=1
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("test", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<ValidatedCfg>("", parser);

    EXPECT_EQ(cfg.test, 1);
}

TEST(StaticReflTestParam, ValidationSuccessOptionalParam)
{
    ///test=1
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("test", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<ValidatedOptionalParamCfg>("", parser);

    EXPECT_EQ(cfg.test, 1);
}
//
TEST(StaticReflTestParam, ValidationError)
{
    ///test=-1
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("test", "-1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter 'test': value can't be negative");
            });
}

TEST(StaticReflTestParam, ValidationErrorOptionalParam)
{
    ///test=-1
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("test", "-1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedOptionalParamCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter 'test': value can't be negative");
            });
}

TEST(StaticReflTestParam, ParamWrongTypeError)
{
    ///test = hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("test", "hello", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<SingleParamCfg>("test = hello", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Couldn't set parameter 'test' value from 'hello'");
            });
}

TEST(StaticReflTestParam, UnkownParamError)
{
    ///foo = 1
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("foo", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};

    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<SingleParamCfg>("foo=1", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Unknown param 'foo'");
            });
}

TEST(StaticReflTestParam, ParamListError)
{
    ///test = [1]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParamList("test", {"1"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<SingleParamCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Parameter 'test': config parameter can't be a list.");
            });
}

TEST(StaticReflTestParam, MissingParamError)
{
    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<SingleParamCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Parameter 'test' is missing.");
            });
}

} //namespace test_param
