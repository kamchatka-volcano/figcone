#include "assert_exception.h"
#include <gtest/gtest.h>
#include <figcone/config.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif


namespace test_dict {

struct DictCfg : public figcone::Config<> {
    FIGCONE_DICT(test);
    FIGCONE_DICT(optTest)();
};

struct NonEmptyValidator{
public:
    template <typename T>
    void operator()(const T& container)
    {
        if (container.empty()) throw figcone::ValidationError{"can't be empty"};
    }
};


struct ValidatedCfg : public figcone::Config<> {
    FIGCONE_DICT(test).checkedWith([](const std::map<std::string, std::string>& dict){
        if (dict.empty()) throw figcone::ValidationError{"can't be empty"};
    });
};

struct ValidatedWithFunctorCfg : public figcone::Config<> {
    FIGCONE_DICT(test).checkedWith<NonEmptyValidator>();
};

#ifdef NAMEOF_AVAILABLE
struct DictCfgWithoutMacro : public figcone::Config<>{
    std::map<std::string, std::string> test = dict<&DictCfgWithoutMacro::test>();
};
#else
struct DictCfgWithoutMacro : public figcone::Config<> {
    std::map<std::string, std::string> test = dict<&DictCfgWithoutMacro::test>("test");
};
#endif

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


TEST(TestDict, MultiParam)
{
    auto cfg = DictCfg{};

///[[test]]
///  testInt = 5
///  testDouble = 5.0
///  testString=foo
///  testMultilineString = "Hello
/// world"
///  testEmpty = ""
///[[optTest]]
/// testInt = 100
    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree.asItem().addNode("test", {1, 1}).asItem();
    testNode.addParam("testInt","5", {2,3});
    testNode.addParam("testDouble","5.0", {3,3});
    testNode.addParam("testString","foo", {4,3});
    testNode.addParam("testMultilineString", "Hello\n world", {5,3});
    testNode.addParam("testEmpty","", {6,3});

    auto& optTestNode = tree.asItem().addNode("optTest", {7, 1}).asItem();
    optTestNode.addParam("testInt","100", {8,3});

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    ASSERT_EQ(cfg.test.size(), 5);
    EXPECT_EQ(cfg.test["testInt"], "5");
    EXPECT_EQ(cfg.test["testDouble"], "5.0");
    EXPECT_EQ(cfg.test["testString"], "foo");
    EXPECT_EQ(cfg.test["testMultilineString"], "Hello\n world");
    EXPECT_EQ(cfg.test["testEmpty"], "");
    ASSERT_EQ(cfg.optTest.size(), 1);
    EXPECT_EQ(cfg.optTest["testInt"], "100");
}

TEST(TestDict, MultiParamWithoutOptional)
{
    auto cfg = DictCfg{};

///[test]
///  testInt = 5
///  testDouble = 5.0
///  testString=foo
///  testMultilineString = "Hello
/// world"
///  testEmpty = ""
    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree.asItem().addNode("test", {1, 1}).asItem();
    testNode.addParam("testInt","5", {2,3});
    testNode.addParam("testDouble","5.0", {3,3});
    testNode.addParam("testString","foo", {4,3});
    testNode.addParam("testMultilineString", "Hello\n world", {5,3});
    testNode.addParam("testEmpty","", {6,3});

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    ASSERT_EQ(cfg.test.size(), 5);
    EXPECT_EQ(cfg.test["testInt"], "5");
    EXPECT_EQ(cfg.test["testDouble"], "5.0");
    EXPECT_EQ(cfg.test["testString"], "foo");
    EXPECT_EQ(cfg.test["testMultilineString"], "Hello\n world");
    EXPECT_EQ(cfg.test["testEmpty"], "");
    ASSERT_EQ(cfg.optTest.size(), 0);

}


TEST(TestDict, ValidationSuccess)
{
    auto cfg = ValidatedCfg{};

///[test]
///  testInt = 5
    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree.asItem().addNode("test", {1, 1});
    testNode.asItem().addParam("testInt","5", {2,3});

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    ASSERT_EQ(cfg.test.size(), 1);
    EXPECT_EQ(cfg.test["testInt"], "5");
}

TEST(TestDict, ValidationFailure)
{
    auto cfg = ValidatedCfg{};

///[test]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addNode("test", {1,1});

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Dictionary 'test': can't be empty");
    });
}

TEST(TestDict, ValidationWithFunctorFailure)
{
    auto cfg = ValidatedWithFunctorCfg{};

///[test]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addNode("test", {1,1});

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Dictionary 'test': can't be empty");
    });
}

TEST(TestDict, ParamWithoutMacro)
{
    auto cfg = DictCfgWithoutMacro{};

///[test]
///  testInt = 5
///  testEmpty = ""
///
    auto tree = figcone::makeTreeRoot();
    auto& testNode =tree.asItem().addNode("test", {1, 1});
    testNode.asItem().addParam("testInt", "5", {2,3});
    testNode.asItem().addParam("testEmpty","", {3,3});

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    ASSERT_EQ(cfg.test.size(), 2);
    EXPECT_EQ(cfg.test["testInt"], "5");
    EXPECT_EQ(cfg.test["testEmpty"], "");
}

TEST(TestDict, EmptyDict)
{
    auto cfg = DictCfg{};

///[test]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addNode("test", {1,1});

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    ASSERT_EQ(cfg.test.size(), 0);
}

TEST(TestDict, NodeListDictError)
{
    auto cfg = DictCfg{};

///[[test]]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addNodeList("test", {1,1});

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Dictionary 'test': config node can't be a list.");
    });
}

TEST(TestDict, MissingDictError)
{
    auto cfg = DictCfg{};

    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Node 'test' is missing.");
    });
}

}