#include "assert_exception.h"
#include <gtest/gtest.h>
#include <figcone/config.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <optional>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif

namespace test_paramlist {

struct CfgInt : public figcone::Config<figcone::NameFormat::CamelCase>
{
    FIGCONE_PARAMLIST(testIntList, std::deque<int>);
};

struct CfgOptInt : public figcone::Config<figcone::NameFormat::CamelCase>
{
    FIGCONE_PARAMLIST(testIntList, std::optional<std::deque<int>>);
};

struct NotEmpty{
    template<typename T>
    void operator()(const T& value)
    {
        if (value.empty())
            throw figcone::ValidationError{"a list can't be empty"};
    }
};

struct HasNoEmptyElements{
    template<typename T>
    void operator()(const T& value)
    {
        if (std::find_if(value.begin(), value.end(), [](const auto& str){
            return str.empty();
        }) != value.end())
            throw figcone::ValidationError{"a list element can't be empty"};
    }
};

struct ValidatedCfg : public figcone::Config<figcone::NameFormat::CamelCase>
{
    FIGCONE_PARAMLIST(testStrList, std::vector<std::string>)
        .ensure([](const std::vector<std::string>& value)
        {
            if (value.empty()) throw figcone::ValidationError{"a list can't be empty"};
        });
};

struct ValidatedWithFunctorCfg : public figcone::Config<figcone::NameFormat::CamelCase>
{
    FIGCONE_PARAMLIST(testStrList, std::vector<std::string>)
    .ensure<NotEmpty>()
    .ensure<HasNoEmptyElements>();
};

#ifdef NAMEOF_AVAILABLE
struct CfgIntWithoutMacro : public figcone::Config<figcone::NameFormat::CamelCase>
{
    std::vector<int> testIntList = paramList<&CfgIntWithoutMacro::testIntList>();
};
#else
struct CfgIntWithoutMacro : public figcone::Config<figcone::NameFormat::CamelCase>
{
    std::vector<int> testIntList = paramList<&CfgIntWithoutMacro::testIntList>("testIntList");
};
#endif

struct CfgStr : public figcone::Config<figcone::NameFormat::CamelCase>
{
    FIGCONE_PARAMLIST(testStrList, std::vector<std::string>);
    FIGCONE_PARAMLIST(testIntList, std::vector<int>)({1, 2, 3});
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


TEST(TestParamList, Basic)
{
    auto cfg = CfgInt{};

///testIntList = [1, 2, 3]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testIntList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    cfg.read("", parser);

    ASSERT_EQ(cfg.testIntList.size(), 3);
    EXPECT_EQ(cfg.testIntList.at(0), 1);
    EXPECT_EQ(cfg.testIntList.at(1), 2);
    EXPECT_EQ(cfg.testIntList.at(2), 3);
}

TEST(TestParamList, BasicOptional)
{
    auto cfg = CfgOptInt{};

///testIntList = [1, 2, 3]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testIntList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    cfg.read("", parser);

    ASSERT_TRUE(cfg.testIntList);
    ASSERT_EQ(cfg.testIntList->size(), 3);
    EXPECT_EQ(cfg.testIntList->at(0), 1);
    EXPECT_EQ(cfg.testIntList->at(1), 2);
    EXPECT_EQ(cfg.testIntList->at(2), 3);
}

TEST(TestParamList, BasicMissingOptional)
{
    auto cfg = CfgOptInt{};

    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};

    cfg.read("", parser);

    ASSERT_FALSE(cfg.testIntList);
}

TEST(TestParamList, BasicWithoutMacro)
{
    auto cfg = CfgIntWithoutMacro{};

///testIntList = [1, 2, 3]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testIntList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    cfg.read("", parser);

    ASSERT_EQ(cfg.testIntList.size(), 3);
    EXPECT_EQ(cfg.testIntList.at(0), 1);
    EXPECT_EQ(cfg.testIntList.at(1), 2);
    EXPECT_EQ(cfg.testIntList.at(2), 3);
}

TEST(TestParamList, ValidationSuccess)
{
    auto cfg = ValidatedCfg{};

///testStrList = [1, 2, 3]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{"1", "2", "3"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    cfg.read("", parser);

    ASSERT_EQ(cfg.testStrList.size(), 3);
    EXPECT_EQ(cfg.testStrList.at(0), "1");
    EXPECT_EQ(cfg.testStrList.at(1), "2");
    EXPECT_EQ(cfg.testStrList.at(2), "3");
}

TEST(TestParamList, ValidationError)
{
    auto cfg = ValidatedCfg{};

///testStrList = []
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter list 'testStrList': a list can't be empty");
    });
}

TEST(TestParamList, ValidationWithFunctorError)
{
    auto cfg = ValidatedWithFunctorCfg{};
///testStrList = []
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter list 'testStrList': a list can't be empty");
    });
}

TEST(TestParamList, ValidationWithFunctorError2)
{
    auto cfg = ValidatedWithFunctorCfg{};
///testStrList = ['hello', '']
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{"hello", ""}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("testStrList = 'hello', ''", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter list 'testStrList': a list element can't be empty");
    });
}

TEST(TestParamList, MissingParamListError)
{
    auto cfg = CfgStr{};

///testIntList = [1, 2]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testIntList", std::vector<std::string>{"1", "2"}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Parameter 'testStrList' is missing.");
    });
}

TEST(TestParamList, ParamNotListError)
{
    auto cfg = CfgInt{};

///testIntList = 1
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testIntList", "1", {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Parameter list 'testIntList': config parameter must be a list.");
    });
}

TEST(TestParamList, DefaultValue)
{
    auto cfg = CfgStr{};

///testStrList = []
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParamList("testStrList", std::vector<std::string>{}, {1, 1});
    auto parser = TreeProvider{std::move(tree)};

    cfg.read("", parser);
    ASSERT_EQ(cfg.testStrList.size(), 0);
    ASSERT_EQ(cfg.testIntList.size(), 3);
    EXPECT_EQ(cfg.testIntList.at(0), 1);
    EXPECT_EQ(cfg.testIntList.at(1), 2);
    EXPECT_EQ(cfg.testIntList.at(2), 3);
}

}
