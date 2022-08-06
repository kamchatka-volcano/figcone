#include "assert_exception.h"
#include <gtest/gtest.h>
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <map>
#include <string>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif


namespace test_dict {

using StringMap = std::map<std::string, std::string>;
using StringUnorderedMap = std::map<std::string, std::string>;
using IntMap = std::map<std::string, int>;

struct DictCfg : public figcone::Config {
    FIGCONE_DICT(test, StringMap);
    FIGCONE_DICT(optTest, StringUnorderedMap)();
    FIGCONE_DICT(optTest2, figcone::optional<StringMap>);
};

struct IntDictCfg : public figcone::Config {
    FIGCONE_DICT(test, IntMap)({{"abc", 11}, {"xyz", 12}});
};

struct NonEmptyValidator{
public:
    template <typename T>
    void operator()(const T& container)
    {
        if (container.empty()) throw figcone::ValidationError{"can't be empty"};
    }
};


struct ValidatedCfg : public figcone::Config {
    FIGCONE_DICT(test, StringMap).checkedWith([](const std::map<std::string, std::string>& dict){
        if (dict.empty()) throw figcone::ValidationError{"can't be empty"};
    });
    FIGCONE_DICT(testOpt, figcone::optional<StringMap>).checkedWith([](const figcone::optional<StringMap>& dict){
        if (dict && dict->empty()) throw figcone::ValidationError{"can't be empty"};
    });
};

struct ValidatedWithFunctorCfg : public figcone::Config {
    FIGCONE_DICT(test, StringMap).checkedWith<NonEmptyValidator>();
};

#ifdef NAMEOF_AVAILABLE
struct DictCfgWithoutMacro : public figcone::Config{
    std::map<std::string, std::string> test = dict<&DictCfgWithoutMacro::test>();
    std::unordered_map<std::string, std::string> optTest = dict<&DictCfgWithoutMacro::optTest>()();
    figcone::optional<std::map<std::string, std::string>> optTest2 = dict<&DictCfgWithoutMacro::optTest2>();
};
#else
struct DictCfgWithoutMacro : public figcone::Config{
    std::map<std::string, std::string> test = dict<&DictCfgWithoutMacro::test>("test");
    std::unordered_map<std::string, std::string> optTest = dict<&DictCfgWithoutMacro::optTest>("optTest")();
    figcone::optional<std::map<std::string, std::string>> optTest2 = dict<&DictCfgWithoutMacro::optTest2>("optTest2");
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

///[[test]]
///  testInt = 5
///  testDouble = 5.0
///  testString=foo
///  testMultilineString = "Hello
/// world"
///  testEmpty = ""
///[[optTest]]
/// testInt = 100
///[[optTest2]]
/// testInt = 200

    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree.asItem().addNode("test", {1, 1}).asItem();
    testNode.addParam("testInt","5", {2,3});
    testNode.addParam("testDouble","5.0", {3,3});
    testNode.addParam("testString","foo", {4,3});
    testNode.addParam("testMultilineString", "Hello\n world", {5,3});
    testNode.addParam("testEmpty","", {6,3});

    auto& optTestNode = tree.asItem().addNode("optTest", {7, 1}).asItem();
    optTestNode.addParam("testInt","100", {8,3});

    auto& optTestNode2 = tree.asItem().addNode("optTest2", {9, 1}).asItem();
    optTestNode2.addParam("testInt","200", {10,3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<DictCfg>("", parser);

    ASSERT_EQ(cfg.test.size(), 5);
    EXPECT_EQ(cfg.test["testInt"], "5");
    EXPECT_EQ(cfg.test["testDouble"], "5.0");
    EXPECT_EQ(cfg.test["testString"], "foo");
    EXPECT_EQ(cfg.test["testMultilineString"], "Hello\n world");
    EXPECT_EQ(cfg.test["testEmpty"], "");
    ASSERT_EQ(cfg.optTest.size(), 1);
    EXPECT_EQ(cfg.optTest["testInt"], "100");
    ASSERT_TRUE(cfg.optTest2);
    ASSERT_EQ(cfg.optTest2->size(), 1);
    EXPECT_EQ(cfg.optTest2->at("testInt"), "200");
}

TEST(TestDict, MultiParamWithoutOptional)
{
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
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<DictCfg>("", parser);


    ASSERT_EQ(cfg.test.size(), 5);
    EXPECT_EQ(cfg.test["testInt"], "5");
    EXPECT_EQ(cfg.test["testDouble"], "5.0");
    EXPECT_EQ(cfg.test["testString"], "foo");
    EXPECT_EQ(cfg.test["testMultilineString"], "Hello\n world");
    EXPECT_EQ(cfg.test["testEmpty"], "");
    ASSERT_EQ(cfg.optTest.size(), 0);
    ASSERT_FALSE(cfg.optTest2);
}

TEST(TestDict, IntMultiParam)
{
///[[test]]
///  foo = 5
///  bar = 42
///  baz = 777

    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree.asItem().addNode("test", {1, 1}).asItem();
    testNode.addParam("foo","5", {2,3});
    testNode.addParam("bar","42", {3,3});
    testNode.addParam("baz","777", {4,3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<IntDictCfg>("", parser);

    ASSERT_EQ(cfg.test.size(), 3);
    EXPECT_EQ(cfg.test["foo"], 5);
    EXPECT_EQ(cfg.test["bar"], 42);
    EXPECT_EQ(cfg.test["baz"], 777);
}

TEST(TestDict, DefaultValue)
{
    auto tree = figcone::makeTreeRoot();

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<IntDictCfg>("", parser);

    ASSERT_EQ(cfg.test.size(), 2);
    EXPECT_EQ(cfg.test["abc"], 11);
    EXPECT_EQ(cfg.test["xyz"], 12);
}


TEST(TestDict, ValidationSuccess)
{
///[test]
///  testInt = 5
///[testOpt]
///  testStr = Foo
    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree.asItem().addNode("test", {1, 1});
    testNode.asItem().addParam("testInt","5", {2,3});
    auto& testNodeOpt = tree.asItem().addNode("testOpt", {3, 1});
    testNodeOpt.asItem().addParam("testStr","Foo", {4,3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<ValidatedCfg>("", parser);

    ASSERT_EQ(cfg.test.size(), 1);
    EXPECT_EQ(cfg.test["testInt"], "5");
    ASSERT_TRUE(cfg.testOpt);
    ASSERT_EQ(cfg.testOpt->size(), 1);
    EXPECT_EQ(cfg.testOpt->at("testStr"), "Foo");
}

TEST(TestDict, ValidationFailure)
{
///[test]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addNode("test", {1,1});
    auto cfgReader = figcone::ConfigReader{};
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        auto cfg = cfgReader.read<ValidatedCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Dictionary 'test': can't be empty");
    });
}

TEST(TestDict, ValidationWithFunctorFailure)
{
///[test]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addNode("test", {1,1});
    auto cfgReader = figcone::ConfigReader{};

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<ValidatedWithFunctorCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Dictionary 'test': can't be empty");
    });
}

TEST(TestDict, ParamWithoutMacro)
{
///[test]
///  testInt = 5
///  testEmpty = ""
///
    auto tree = figcone::makeTreeRoot();
    auto& testNode =tree.asItem().addNode("test", {1, 1});
    testNode.asItem().addParam("testInt", "5", {2,3});
    testNode.asItem().addParam("testEmpty","", {3,3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<DictCfgWithoutMacro>("", parser);

    ASSERT_EQ(cfg.test.size(), 2);
    EXPECT_EQ(cfg.test["testInt"], "5");
    EXPECT_EQ(cfg.test["testEmpty"], "");
    ASSERT_EQ(cfg.optTest.size(), 0);
    ASSERT_FALSE(cfg.optTest2);
}

TEST(TestDict, EmptyDict)
{

///[test]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addNode("test", {1,1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<DictCfg>("", parser);

    ASSERT_EQ(cfg.test.size(), 0);
}

TEST(TestDict, NodeListDictError)
{
///[[test]]
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addNodeList("test", {1,1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    assert_exception<figcone::ConfigError>([&] {
        auto cfg = cfgReader.read<DictCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Dictionary 'test': config node can't be a list.");
    });
}

TEST(TestDict, MissingDictError)
{
    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    assert_exception<figcone::ConfigError>([&] {
        auto cfg = cfgReader.read<DictCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Node 'test' is missing.");
    });
}

}