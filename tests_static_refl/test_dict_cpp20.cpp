#include "assert_exception.h"
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <gtest/gtest.h>
#include <map>
#include <string>

namespace test_dict {

using StringMap = std::map<std::string, std::string>;
using StringUnorderedMap = std::map<std::string, std::string>;
using IntMap = std::map<std::string, int>;

struct DictCfg {
    std::map<std::string, std::string> test;
    std::unordered_map<std::string, std::string> optTest;
    std::optional<std::map<std::string, std::string>> optTest2;

    using traits = figcone::FieldTraits<figcone::OptionalField<&DictCfg::optTest>>;
};

struct IntDictCfg {
    std::map<std::string, int> test = {{"abc", 11}, {"xyz", 12}};

    using traits = figcone::FieldTraits<figcone::OptionalField<&IntDictCfg::test>>;
};

struct NonEmptyValidator {
public:
    template<typename T>
    void operator()(const T& container)
    {
        if (container.empty())
            throw figcone::ValidationError{"can't be empty"};
    }
};

struct ValidatedCfg {
    std::map<std::string, std::string> test;

    using traits = figcone::FieldTraits<figcone::ValidatedField<&ValidatedCfg::test, NonEmptyValidator>>;
};

struct ValidatedOptionalDictCfg {
    std::optional<std::map<std::string, std::string>> testOpt;

    using traits = figcone::FieldTraits<figcone::ValidatedField<&ValidatedOptionalDictCfg::testOpt, NonEmptyValidator>>;
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

TEST(StaticReflTestDict, MultiParam)
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
    auto& testNode = tree->asItem().addNode("test", {1, 1}).asItem();
    testNode.addParam("testInt", "5", {2, 3});
    testNode.addParam("testDouble", "5.0", {3, 3});
    testNode.addParam("testString", "foo", {4, 3});
    testNode.addParam("testMultilineString", "Hello\n world", {5, 3});
    testNode.addParam("testEmpty", "", {6, 3});

    auto& optTestNode = tree->asItem().addNode("optTest", {7, 1}).asItem();
    optTestNode.addParam("testInt", "100", {8, 3});

    auto& optTestNode2 = tree->asItem().addNode("optTest2", {9, 1}).asItem();
    optTestNode2.addParam("testInt", "200", {10, 3});

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

TEST(StaticReflTestDict, MultiParamWithoutOptional)
{
    ///[test]
    ///  testInt = 5
    ///  testDouble = 5.0
    ///  testString=foo
    ///  testMultilineString = "Hello
    /// world"
    ///  testEmpty = ""
    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree->asItem().addNode("test", {1, 1}).asItem();
    testNode.addParam("testInt", "5", {2, 3});
    testNode.addParam("testDouble", "5.0", {3, 3});
    testNode.addParam("testString", "foo", {4, 3});
    testNode.addParam("testMultilineString", "Hello\n world", {5, 3});
    testNode.addParam("testEmpty", "", {6, 3});

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

TEST(StaticReflTestDict, IntMultiParam)
{
    ///[[test]]
    ///  foo = 5
    ///  bar = 42
    ///  baz = 777

    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree->asItem().addNode("test", {1, 1}).asItem();
    testNode.addParam("foo", "5", {2, 3});
    testNode.addParam("bar", "42", {3, 3});
    testNode.addParam("baz", "777", {4, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<IntDictCfg>("", parser);

    ASSERT_EQ(cfg.test.size(), 3);
    EXPECT_EQ(cfg.test["foo"], 5);
    EXPECT_EQ(cfg.test["bar"], 42);
    EXPECT_EQ(cfg.test["baz"], 777);
}

TEST(StaticReflTestDict, DefaultValue)
{
    auto tree = figcone::makeTreeRoot();

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<IntDictCfg>("", parser);

    ASSERT_EQ(cfg.test.size(), 2);
    EXPECT_EQ(cfg.test["abc"], 11);
    EXPECT_EQ(cfg.test["xyz"], 12);
}

TEST(StaticReflTestDict, ValidationSuccess)
{
    ///[test]
    ///  testInt = 5
    ///[testOpt]
    ///  testStr = Foo
    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree->asItem().addNode("test", {1, 1});
    testNode.asItem().addParam("testInt", "5", {2, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<ValidatedCfg>("", parser);

    ASSERT_EQ(cfg.test.size(), 1);
    EXPECT_EQ(cfg.test["testInt"], "5");
}

TEST(StaticReflTestDict, ValidationSuccessOptionalDict)
{
    ///[test]
    ///  testInt = 5
    ///[testOpt]
    ///  testStr = Foo
    auto tree = figcone::makeTreeRoot();
    auto& testNodeOpt = tree->asItem().addNode("testOpt", {3, 1});
    testNodeOpt.asItem().addParam("testStr", "Foo", {4, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<ValidatedOptionalDictCfg>("", parser);

    ASSERT_TRUE(cfg.testOpt.has_value());
    ASSERT_EQ(cfg.testOpt->size(), 1);
    EXPECT_EQ(cfg.testOpt->at("testStr"), "Foo");
}

TEST(StaticReflTestDict, ValidationFailure)
{
    ///[test]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addNode("test", {1, 1});
    auto cfgReader = figcone::ConfigReader{};
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                auto cfg = cfgReader.read<ValidatedCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Dictionary 'test': can't be empty");
            });
}

TEST(StaticReflTestDict, ValidationFailureOptionalDict)
{
    ///[test]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addNode("testOpt", {1, 1});
    auto cfgReader = figcone::ConfigReader{};
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                auto cfg = cfgReader.read<ValidatedOptionalDictCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Dictionary 'testOpt': can't be empty");
            });
}

TEST(StaticReflTestDict, EmptyDict)
{

    ///[test]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addNode("test", {1, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    auto cfg = cfgReader.read<DictCfg>("", parser);

    ASSERT_EQ(cfg.test.size(), 0);
}

TEST(StaticReflTestDict, NodeListDictError)
{
    ///[[test]]
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addNodeList("test", {1, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                auto cfg = cfgReader.read<DictCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Dictionary 'test': config node can't be a list.");
            });
}

TEST(StaticReflTestDict, MissingDictError)
{
    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                auto cfg = cfgReader.read<DictCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Node 'test' is missing.");
            });
}

} //namespace test_dict