#include "assert_exception.h"
#include <gtest/gtest.h>
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/stringconverter.h>
#include <figcone_tree/tree.h>
#include <filesystem>
#include <optional>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif

namespace test_param {

#ifdef NAMEOF_AVAILABLE
struct SingleParamCfgWithoutMacro : public figcone::Config{
    int test = param<&SingleParamCfgWithoutMacro::test>();
};
#else
struct SingleParamCfgWithoutMacro : public figcone::Config{
    int test = param<&SingleParamCfgWithoutMacro::test>("test");
};
#endif

struct SingleParamCfg : public figcone::Config{
    FIGCONE_PARAM(test, int);
};

struct IsPositive{
public:
    void operator()(int val)
    {
        if (val < 0) throw figcone::ValidationError{"value can't be negative"};
    }
};

struct ValidatedCfg : public figcone::Config{
    FIGCONE_PARAM(test, int).ensure(
            [](int val){
                if (val < 0) throw figcone::ValidationError{"value can't be negative"};
            });
};

struct ValidatedWithFunctorCfg : public figcone::Config{
    FIGCONE_PARAM(test, int).ensure<IsPositive>();
};

struct StringParamCfg : public figcone::Config{
    FIGCONE_PARAM(test, std::string);
    FIGCONE_PARAM(testFs, std::filesystem::path)("default.txt");
};

struct MultiParamCfg : public figcone::Config{
    FIGCONE_PARAM(testInt, int);
    FIGCONE_PARAM(testDouble, double)(9.0);
    FIGCONE_PARAM(testString, figcone::optional<std::string>)();
    FIGCONE_PARAM(testString2, figcone::optional<std::string>);
};

struct UserType {
    std::string value;
};
}

template<>
std::optional<test_param::UserType> figcone::StringConverter<test_param::UserType>::fromString(const std::string& data)
{
    auto result = test_param::UserType{};
    result.value = data;
    return result;
}

namespace test_param{
struct SingleUserTypeParamCfg : public figcone::Config{
    FIGCONE_PARAM(test, UserType);
};

class TreeProvider : public figcone::IParser{
public:
    TreeProvider(figcone::TreeNode tree)
    : tree_(std::move(tree)) {}

    figcone::TreeNode parse(std::istream&) override
    {
        return tree_;
    }

    figcone::TreeNode tree_;
};


TEST(TestParam, IntParam)
{
///test=1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<SingleParamCfg>("", parser);

    EXPECT_EQ(cfg.test, 1);
}

TEST(TestParam, StringParam)
{
///test=hello
///testFs='hello world.txt'
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "hello", {1, 1});
    tree.asItem().addParam("testFs", "hello world.txt", {2, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<StringParamCfg>("", parser);

    EXPECT_EQ(cfg.test, "hello");
    EXPECT_EQ(cfg.testFs, "hello world.txt");
}

TEST(TestParam, WithoutMacro)
{
///test=1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<SingleParamCfgWithoutMacro>("", parser);

    EXPECT_EQ(cfg.test, 1);
}

TEST(TestParam, UserTypeParam)
{
///test='hello world'
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "hello world", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<SingleUserTypeParamCfg>("", parser);

    EXPECT_EQ(cfg.test.value, "hello world");
}

TEST(TestParam, EmptyStringParam)
{
///test=''
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<StringParamCfg>("", parser);

    EXPECT_EQ(cfg.test, "");
}
//
TEST(TestParam, MultiParam)
{
///testInt=5
///testDouble=5.0
///testString='foo'
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testInt", "5", {1, 1});
    tree.asItem().addParam("testDouble", "5.0", {2, 1});
    tree.asItem().addParam("testString", "foo", {3, 1});
    tree.asItem().addParam("testString2", "Hello world", {4, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<MultiParamCfg>("", parser);

    EXPECT_EQ(cfg.testInt, 5);
    EXPECT_EQ(cfg.testDouble, 5.0);
    ASSERT_TRUE(cfg.testString.has_value());
    EXPECT_EQ(*cfg.testString, "foo");
    ASSERT_TRUE(cfg.testString2.has_value());
    EXPECT_EQ(*cfg.testString2, "Hello world");
}

TEST(TestParam, MultiParamUnspecifiedOptionals)
{
///testInt=5
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testInt", "5", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<MultiParamCfg>("", parser);

    EXPECT_EQ(cfg.testInt, 5);
    EXPECT_EQ(cfg.testDouble, 9.0);
    ASSERT_FALSE(cfg.testString.has_value());
    ASSERT_FALSE(cfg.testString2.has_value());
}

TEST(TestParam, ValidationSuccess)
{
///test=1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<ValidatedCfg>("", parser);

    EXPECT_EQ(cfg.test, 1);
}

TEST(TestParam, ValidationError)
{
///test=-1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "-1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<ValidatedCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter 'test': value can't be negative");
    });
}

TEST(TestParam, ValidationWithFinctorError)
{
///test=-1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "-1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<ValidatedWithFunctorCfg>("test = -1", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter 'test': value can't be negative");
    });
}

TEST(TestParam, ParamWrongTypeError)
{
///test = hello
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "hello", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<SingleParamCfg>("test = hello", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Couldn't set parameter 'test' value from 'hello'");
    });
}

TEST(TestParam, UnkownParamError)
{
///foo = 1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};

    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<SingleParamCfg>("foo=1", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Unknown param 'foo'");
    });
}

TEST(TestParam, ParamListError)
{
///test = [1]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("test", {"1"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<SingleParamCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter 'test': config parameter can't be a list.");
    });
}


TEST(TestParam, MissingParamError)
{
    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<SingleParamCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Parameter 'test' is missing.");
    });
}

}
