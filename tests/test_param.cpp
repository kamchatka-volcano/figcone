#include "assert_exception.h"
#include <gtest/gtest.h>
#include <figcone/config.h>
#include <figcone/errors.h>
#include <figcone/stringconverter.h>
#include <figcone_tree/tree.h>
#include <filesystem>
#include <optional>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif

namespace test_param {

#ifdef NAMEOF_AVAILABLE
struct SingleParamCfgWithoutMacro : public figcone::Config<figcone::NameFormat::CamelCase> {
    int test = param<&SingleParamCfgWithoutMacro::test>();
};
#else
struct SingleParamCfgWithoutMacro : public figcone::Config<figcone::NameFormat::CamelCase> {
    int test = param<&SingleParamCfgWithoutMacro::test>("test");
};
#endif

struct SingleParamCfg : public figcone::Config<figcone::NameFormat::CamelCase> {
    FIGCONE_PARAM(test, int);
};

struct IsPositive{
public:
    void operator()(int val)
    {
        if (val < 0) throw figcone::ValidationError{"value can't be negative"};
    }
};

struct ValidatedCfg : public figcone::Config<figcone::NameFormat::CamelCase> {
    FIGCONE_PARAM(test, int).ensure(
            [](int val){
                if (val < 0) throw figcone::ValidationError{"value can't be negative"};
            });
};

struct ValidatedWithFunctorCfg : public figcone::Config<figcone::NameFormat::CamelCase> {
    FIGCONE_PARAM(test, int).ensure<IsPositive>();
};

struct StringParamCfg : public figcone::Config<figcone::NameFormat::CamelCase> {
    FIGCONE_PARAM(test, std::string);
    FIGCONE_PARAM(testFs, std::filesystem::path)("default.txt");
};

struct MultiParamCfg : public figcone::Config<figcone::NameFormat::CamelCase> {
    FIGCONE_PARAM(testInt, int);
    FIGCONE_PARAM(testDouble, double)(9.0);
    FIGCONE_PARAM(testString, std::optional<std::string>)();
    FIGCONE_PARAM(testString2, std::optional<std::string>);
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
struct SingleUserTypeParamCfg : public figcone::Config<figcone::NameFormat::CamelCase>{
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
    auto cfg = SingleParamCfg{};

///test=1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    EXPECT_EQ(cfg.test, 1);
}

TEST(TestParam, StringParam)
{
    auto cfg = StringParamCfg{};

///test=hello
///testFs='hello world.txt'
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "hello", {1, 1});
    tree.asItem().addParam("testFs", "hello world.txt", {2, 1});
    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    EXPECT_EQ(cfg.test, "hello");
    EXPECT_EQ(cfg.testFs, "hello world.txt");
}

TEST(TestParam, WithoutMacro)
{
    auto cfg = SingleParamCfgWithoutMacro{};

///test=1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    EXPECT_EQ(cfg.test, 1);
}

TEST(TestParam, UserTypeParam)
{
    auto cfg = SingleUserTypeParamCfg{};

///test='hello world'
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "hello world", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    EXPECT_EQ(cfg.test.value, "hello world");
}

TEST(TestParam, EmptyStringParam)
{
    auto cfg = StringParamCfg{};

///test=''
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    EXPECT_EQ(cfg.test, "");
}

TEST(TestParam, MultiParam)
{
    auto cfg = MultiParamCfg{};

///testInt=5
///testDouble=5.0
///testString='foo'
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testInt", "5", {1, 1});
    tree.asItem().addParam("testDouble", "5.0", {2, 1});
    tree.asItem().addParam("testString", "foo", {3, 1});
    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    EXPECT_EQ(cfg.testInt, 5);
    EXPECT_EQ(cfg.testDouble, 5.0);
    ASSERT_TRUE(cfg.testString.has_value());
    EXPECT_EQ(*cfg.testString, "foo");
    ASSERT_FALSE(cfg.testString2.has_value());
}

TEST(TestParam, MultiParamUnspecifiedOptionals)
{
    auto cfg = MultiParamCfg{};

///testInt=5
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testInt", "5", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    EXPECT_EQ(cfg.testInt, 5);
    EXPECT_EQ(cfg.testDouble, 9.0);
    ASSERT_FALSE(cfg.testString.has_value());
    ASSERT_FALSE(cfg.testString2.has_value());
}

TEST(TestParam, ValidationSuccess)
{
    auto cfg = ValidatedCfg{};

///test=1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    EXPECT_EQ(cfg.test, 1);
}

TEST(TestParam, ValidationError)
{
    auto cfg = ValidatedCfg{};

///test=-1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "-1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter 'test': value can't be negative");
    });
}

TEST(TestParam, ValidationWithFinctorError)
{
    auto cfg = ValidatedWithFunctorCfg{};

///test=-1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "-1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("test = -1", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter 'test': value can't be negative");
    });
}

TEST(TestParam, ParamWrongTypeError)
{
    auto cfg = SingleParamCfg{};

///test = hello
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test", "hello", {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("test = hello", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Couldn't set parameter 'test' value from 'hello'");
    });
}

TEST(TestParam, UnkownParamError)
{
    auto cfg = SingleParamCfg{};

///foo = 1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("foo=1", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Unknown param 'foo'");
    });
}

TEST(TestParam, ParamListError)
{
    auto cfg = SingleParamCfg{};

///test = [1]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("test", {"1"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter 'test': config parameter can't be a list.");
    });
}


TEST(TestParam, MissingParamError)
{
    auto cfg = SingleParamCfg{};

    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Parameter 'test' is missing.");
    });
}

}
