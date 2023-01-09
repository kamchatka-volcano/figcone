#include "assert_exception.h"
#include <gtest/gtest.h>
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <optional>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif


namespace test_node {

struct NonAggregateA : public figcone::Config {
  using Config::Config;
  virtual ~NonAggregateA() = default;
  FIGCONE_PARAM(testInt, int);
};

struct A : public figcone::Config {
    FIGCONE_PARAM(testInt, int);
};

struct B : public figcone::Config {
    FIGCONE_PARAM(testInt, int);
    FIGCONE_PARAM(testString, std::string)();
};

struct C : public figcone::Config {
    FIGCONE_PARAM(testInt, int)(9);
    FIGCONE_PARAM(testDouble, double)(9.0);
    FIGCONE_NODE(b, B);
};

struct D : public figcone::Config {
    FIGCONE_PARAM(testInt, int)();
};

struct NonAggregateSingleNodeSingleLevelCfg : public figcone::Config {
    using Config::Config;
    FIGCONE_PARAM(foo, int);
    FIGCONE_PARAM(bar, std::string);
    FIGCONE_NODE(a, NonAggregateA);

    virtual ~NonAggregateSingleNodeSingleLevelCfg() = default;
};


struct SingleNodeSingleLevelCfg : public figcone::Config {
    FIGCONE_PARAM(foo, int);
    FIGCONE_PARAM(bar, std::string);
    FIGCONE_NODE(a, A);
};

struct OptionalNodeCfg : public figcone::Config {
    FIGCONE_PARAM(foo, int);
    FIGCONE_PARAM(bar, std::string);
    FIGCONE_NODE(d, D)();
};

struct HasNonEmptyFields{
public:
    void operator()(const B& b)
    {
        if (b.testInt == 0 && b.testString.empty()) throw figcone::ValidationError{"both fields can't be empty"};
    }
};

struct ValidatedNodeCfg : public figcone::Config {
    FIGCONE_NODE(b, B).ensure([](const B& b){
        if (b.testInt == 0 && b.testString.empty()) throw figcone::ValidationError{"both fields can't be empty"};
    });
};

struct ValidatedWithFunctorNodeCfg : public figcone::Config {
    FIGCONE_NODE(b, B).ensure<HasNonEmptyFields>();
};


struct MultiNodeSingleLevelCfg : public figcone::Config {
    FIGCONE_PARAM(foo, int);
    FIGCONE_PARAM(bar, std::string);
    FIGCONE_NODE(a, A);
    FIGCONE_NODE(b, figcone::optional<B>);
};

#ifdef NAMEOF_AVAILABLE
struct SingleNodeCfgWithoutMacro : public figcone::Config{
    A a = node<&SingleNodeCfgWithoutMacro::a>();
};
#else
struct SingleNodeCfgWithoutMacro : public figcone::Config {
    A a = node<&SingleNodeCfgWithoutMacro::a>("a");
};
#endif


class TreeProvider : public figcone::IParser{
public:
    TreeProvider(figcone::TreeNode tree)
    : tree_(std::move(tree)) {}

    figcone::TreeNode parse(std::istream&) override
    {
        return std::move(tree_);
    }

    figcone::TreeNode tree_;
};


TEST(TestNode, SingleNodeSingleLevel)
{

///
///foo = 5
///bar = test
///[a]
///  testInt = 10
///
    auto makeTreeProvider = [] {
        auto tree = figcone::makeTreeRoot();
        tree.asItem().addParam("foo", "5", {1, 1});
        tree.asItem().addParam("bar", "test", {2, 1});
        auto& aNode = tree.asItem().addNode("a", {3, 1});
        aNode.asItem().addParam("testInt", "10", {4, 1});
        return TreeProvider{std::move(tree)};
    };

    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto parser = makeTreeProvider();
    auto cfg = cfgReader.read<SingleNodeSingleLevelCfg>("", parser);
    auto parser2 = makeTreeProvider();
    auto cfg2 = cfgReader.read<SingleNodeSingleLevelCfg>("", parser2);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.a.testInt, 10);

    EXPECT_EQ(cfg2.foo, 5);
    EXPECT_EQ(cfg2.bar, "test");
    EXPECT_EQ(cfg2.a.testInt, 10);
}

TEST(TestNode, NonAggregateSingleNodeSingleLevel)
{
    ///
    ///foo = 5
    ///bar = test
    ///[a]
    ///  testInt = 10
    ///
    auto makeTreeProvider = [] {
      auto tree = figcone::makeTreeRoot();
      tree.asItem().addParam("foo", "5", {1, 1});
      tree.asItem().addParam("bar", "test", {2, 1});
      auto& aNode = tree.asItem().addNode("a", {3, 1});
      aNode.asItem().addParam("testInt", "10", {4, 1});
      return TreeProvider{std::move(tree)};
    };

    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto parser = makeTreeProvider();
    auto cfg = cfgReader.read<NonAggregateSingleNodeSingleLevelCfg>("", parser);
    auto parser2 = makeTreeProvider();
    auto cfg2 = cfgReader.read<NonAggregateSingleNodeSingleLevelCfg>("", parser2);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.a.testInt, 10);

    EXPECT_EQ(cfg2.foo, 5);
    EXPECT_EQ(cfg2.bar, "test");
    EXPECT_EQ(cfg2.a.testInt, 10);
}


TEST(TestNode, OptionalNode)
{
///
///foo = 5
///bar = test
///[d]
///  testInt = 10
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "5", {1, 1});
    tree.asItem().addParam("bar", "test", {2, 1});
    auto& aNode = tree.asItem().addNode("d", {3, 1});
    aNode.asItem().addParam("testInt", "10", {4, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<OptionalNodeCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.d.testInt, 10);
}

TEST(TestNode, WithoutOptionalNode)
{
///
///foo = 5
///bar = test
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "5", {1, 1});
    tree.asItem().addParam("bar", "test", {2, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<OptionalNodeCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.d.testInt, 0);
}

TEST(TestNode, SingleNodeWithoutMacro)
{
///
///[a]
/// testInt = 10
///
    auto tree = figcone::makeTreeRoot();
    auto& aNode = tree.asItem().addNode("a", {1, 1});
    aNode.asItem().addParam("testInt", "10", {2, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<SingleNodeCfgWithoutMacro>("", parser);

    EXPECT_EQ(cfg.a.testInt, 10);
}

TEST(TestNode, ValidationSuccess)
{
///
///[b]
///  testInt = 10
///  testString = ""
///
    auto tree = figcone::makeTreeRoot();
    auto& bNode = tree.asItem().addNode("b", {1, 1});;
    bNode.asItem().addParam("testInt", "10", {2, 3});
    bNode.asItem().addParam("testString", "", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<ValidatedNodeCfg>("", parser);

    EXPECT_EQ(cfg.b.testInt, 10);
    EXPECT_EQ(cfg.b.testString, "");
}

TEST(TestNode, ValidationError)
{
///[b]
///  testInt = 0
///  testString = "":
///
    auto tree = figcone::makeTreeRoot();
    auto& bNode = tree.asItem().addNode("b", {1, 1});
    bNode.asItem().addParam("testInt", "0", {2, 3});
    bNode.asItem().addParam("testString", "", {3, 3});


    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>([&] {
         cfgReader.read<ValidatedNodeCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Node 'b': both fields can't be empty");
    });
}

TEST(TestNode, ValidationWithFunctorError)
{
///[b]
///  testInt = 0
///  testString = "":
///
    auto tree = figcone::makeTreeRoot();
    auto& bNode = tree.asItem().addNode("b", {1, 1});
    bNode.asItem().addParam("testInt", "0", {2, 3});
    bNode.asItem().addParam("testString", "", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>([&] {
         cfgReader.read<ValidatedWithFunctorNodeCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Node 'b': both fields can't be empty");
    });
}

TEST(TestNode, MultiNodeSingleLevel)
{
    //auto cfg = MultiNodeSingleLevelCfg{};

///foo = 5
///bar = test
///[a]
///  testInt = 10
///
///[b]
///  testInt = 11
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "5", {1, 1});
    tree.asItem().addParam("bar", "test", {2, 1});
    auto& aNode = tree.asItem().addNode("a", {3, 1});
    aNode.asItem().addParam("testInt", "10", {4, 3});

    auto& bNode = tree.asItem().addNode("b", {5, 1});
    bNode.asItem().addParam("testInt", "11", {6, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<MultiNodeSingleLevelCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.a.testInt, 10);
    ASSERT_TRUE(cfg.b);
    EXPECT_EQ(cfg.b->testInt, 11);
    EXPECT_EQ(cfg.b->testString, "");
}

TEST(TestNode, MultiNodeSingleLevelWithoutOptionalNode)
{

///foo = 5
///bar = test
///[a]
///  testInt = 10
///
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "5", {1, 1});
    tree.asItem().addParam("bar", "test", {2, 1});
    auto& aNode = tree.asItem().addNode("a", {3, 1});
    aNode.asItem().addParam("testInt", "10", {4, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<MultiNodeSingleLevelCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.a.testInt, 10);
    ASSERT_FALSE(cfg.b);
}

struct MultiLevelCfg : public figcone::Config {
    FIGCONE_PARAM(foo, int);
    FIGCONE_PARAM(bar, std::string);
    FIGCONE_NODE(b, B);
    FIGCONE_NODE(c, C);
};

TEST(TestNode, MultiLevel)
{
///foo = 5
///bar = test
///[c]
///  testInt = 11
///  testDouble = 12
///  [c.b]
///    testInt = 10
///    testString = 'Hello world'
///
///[b]
///  testInt = 9
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "5", {1, 1});
    tree.asItem().addParam("bar", "test", {2, 1});
    auto& cNode = tree.asItem().addNode("c", {3, 1});
    cNode.asItem().addParam("testInt", "11", {4, 3});
    cNode.asItem().addParam("testDouble", "12", {5, 3});
    auto& bNode = cNode.asItem().addNode("b", {6, 1});
    bNode.asItem().addParam("testInt", "10", {7, 3});
    bNode.asItem().addParam("testString", "Hello world", {8, 3});
    auto& bNode2 = tree.asItem().addNode("b", {10, 1});
    bNode2.asItem().addParam("testInt", "9", {11, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto cfg = cfgReader.read<MultiLevelCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.b.testInt, 9);
    EXPECT_EQ(cfg.b.testString, "");
    EXPECT_EQ(cfg.c.testInt, 11);
    EXPECT_EQ(cfg.c.testDouble, 12);
    EXPECT_EQ(cfg.c.b.testInt, 10);
    EXPECT_EQ(cfg.c.b.testString, "Hello world");
}

TEST(TestNode, UnknownNodeError)
{
///[test]
///  foo = bar
///
    auto tree = figcone::makeTreeRoot();
    auto& testNode = tree.asItem().addNode("test", {1, 1});
    testNode.asItem().addParam("foo", "bar", {2, 3});
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<SingleNodeSingleLevelCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Unknown node 'test'");
    });
}

TEST(TestNode, NodeListError)
{
///foo = 5
///bar = test
///[c]
///  testInt = 11
///  testDouble = 12
///  [c.b.0]
///    testInt = 10
///    testString = 'Hello world'
///
///[b]
///  testInt = 9
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "5", {1, 1});
    tree.asItem().addParam("bar", "test", {2, 1});
    auto& cNode = tree.asItem().addNode("c", {3, 1});
    cNode.asItem().addParam("testInt", "11", {4, 3});
    cNode.asItem().addParam("testDouble", "12", {5, 3});
    cNode.asItem().addNodeList("b", {6, 1});

    auto& bNode2 = tree.asItem().addNode("b", {10, 1});
    bNode2.asItem().addParam("testInt", "9", {11, 3});

    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<MultiLevelCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:6, column:1] Node 'b': config node can't be a list.");
    });
}

TEST(TestNode, MissingNodeError)
{
///foo = 5
///bar = test
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "5", {1, 1});
    tree.asItem().addParam("bar", "test", {2, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<SingleNodeSingleLevelCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Node 'a' is missing.");
    });
}

TEST(TestNode, MissingNodeError2)
{
///foo = 5
///bar = test
///[c]
///  testInt = 11
///  testDouble = 12
///
///[b]
///  testInt = 9
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("foo", "5", {1, 1});
    tree.asItem().addParam("bar", "test", {2, 1});
    auto& cNode = tree.asItem().addNode("c", {3, 1});
    cNode.asItem().addParam("testInt", "11", {4, 3});
    cNode.asItem().addParam("testDouble", "12", {5, 3});

    auto& bNode = tree.asItem().addNode("b", {7, 1});
    bNode.asItem().addParam("testInt", "9", {8, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader<figcone::NameFormat::CamelCase>{};
    assert_exception<figcone::ConfigError>([&] {
        cfgReader.read<MultiLevelCfg>("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:3, column:1] Node 'c': Node 'b' is missing.");
    });
}

}

