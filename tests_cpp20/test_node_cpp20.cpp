#include "assert_exception.h"
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <gtest/gtest.h>
#include <optional>

namespace test_node {

struct A {
    int testInt;
};

struct B {
    int testInt;
    std::string testString;

    using traits = figcone::FieldTraits< //
            figcone::OptionalField<&B::testString>>;
};

struct C {
    int testInt = 9;
    double testDouble = 9.0;
    B b;

    using traits = figcone::FieldTraits< //
            figcone::OptionalField<&C::testInt>,
            figcone::OptionalField<&C::testDouble> //
            >;
};

struct D {
    int testInt = 0;

    using traits = figcone::FieldTraits< //
            figcone::OptionalField<&D::testInt>>;
};

struct SingleNodeSingleLevelCfg {
    int foo;
    std::string bar;
    A a;
};

struct OptionalNodeCfg {
    int foo;
    std::string bar;
    D d;

    using traits = figcone::FieldTraits< //
            figcone::OptionalField<&OptionalNodeCfg::d>>;
};

struct HasNonEmptyFields {
public:
    void operator()(const B& b)
    {
        if (b.testInt == 0 && b.testString.empty())
            throw figcone::ValidationError{"both fields can't be empty"};
    }
};

struct ValidatedNodeCfg {
    B b;

    using traits = figcone::FieldTraits<figcone::ValidatedField<&ValidatedNodeCfg::b, HasNonEmptyFields>>;
};

struct ValidatedOptionalNodeCfg {
    std::optional<B> b;

    using traits = figcone::FieldTraits<figcone::ValidatedField<&ValidatedOptionalNodeCfg::b, HasNonEmptyFields>>;
};

struct MultiNodeSingleLevelCfg {
    int foo;
    std::string bar;
    A a;
    std::optional<B> b;
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

TEST(StaticReflTestNode, SingleNodeSingleLevel)
{

    ///
    ///foo = 5
    ///bar = test
    ///[a]
    ///  testInt = 10
    ///
    auto makeTreeProvider = []
    {
        auto tree = figcone::makeTreeRoot();
        tree->asItem().addParam("foo", "5", {1, 1});
        tree->asItem().addParam("bar", "test", {2, 1});
        auto& aNode = tree->asItem().addNode("a", {3, 1});
        aNode.asItem().addParam("testInt", "10", {4, 1});
        return TreeProvider{std::move(tree)};
    };

    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
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

TEST(StaticReflTestNode, SingleAnyNodeSingleLevel)
{

    ///
    ///foo = 5
    ///bar = test
    ///[a]
    ///  testInt = 10
    ///
    auto makeTreeProvider = []
    {
        auto tree = figcone::makeTreeRoot();
        tree->asItem().addParam("foo", "5", {1, 1});
        tree->asItem().addParam("bar", "test", {2, 1});
        auto& aNode = tree->asItem().addAny("a", {3, 1});
        aNode.asItem().addParam("testInt", "10", {4, 1});
        return TreeProvider{std::move(tree)};
    };

    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
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

TEST(StaticReflTestNode, OptionalNode)
{
    ///
    ///foo = 5
    ///bar = test
    ///[d]
    ///  testInt = 10
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& aNode = tree->asItem().addNode("d", {3, 1});
    aNode.asItem().addParam("testInt", "10", {4, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<OptionalNodeCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.d.testInt, 10);
}

TEST(StaticReflTestNode, OptionalAnyNode)
{
    ///
    ///foo = 5
    ///bar = test
    ///[d]
    ///  testInt = 10
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& aNode = tree->asItem().addAny("d", {3, 1});
    aNode.asItem().addParam("testInt", "10", {4, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<OptionalNodeCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.d.testInt, 10);
}

TEST(StaticReflTestNode, WithoutOptionalNode)
{
    ///
    ///foo = 5
    ///bar = test
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<OptionalNodeCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.d.testInt, 0);
}

//TEST(StaticReflTestNode, SingleNodeWithoutMacro)
//{
//    ///
//    ///[a]
//    /// testInt = 10
//    ///
//    auto tree = figcone::makeTreeRoot();
//    auto& aNode = tree->asItem().addNode("a", {1, 1});
//    aNode.asItem().addParam("testInt", "10", {2, 3});
//
//    auto parser = TreeProvider{std::move(tree)};
//    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
//    auto cfg = cfgReader.read<SingleNodeCfgWithoutMacro>("", parser);
//
//    EXPECT_EQ(cfg.a.testInt, 10);
//}

TEST(StaticReflTestNode, ValidationSuccess)
{
    ///
    ///[b]
    ///  testInt = 10
    ///  testString = ""
    ///
    auto tree = figcone::makeTreeRoot();
    auto& bNode = tree->asItem().addNode("b", {1, 1});
    ;
    bNode.asItem().addParam("testInt", "10", {2, 3});
    bNode.asItem().addParam("testString", "", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<ValidatedNodeCfg>("", parser);

    EXPECT_EQ(cfg.b.testInt, 10);
    EXPECT_EQ(cfg.b.testString, "");
}

TEST(StaticReflTestNode, AnyNodeValidationSuccess)
{
    ///
    ///[b]
    ///  testInt = 10
    ///  testString = ""
    ///
    auto tree = figcone::makeTreeRoot();
    auto& bNode = tree->asItem().addAny("b", {1, 1});
    ;
    bNode.asItem().addParam("testInt", "10", {2, 3});
    bNode.asItem().addParam("testString", "", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<ValidatedNodeCfg>("", parser);

    EXPECT_EQ(cfg.b.testInt, 10);
    EXPECT_EQ(cfg.b.testString, "");
}

TEST(StaticReflTestNode, ValidationSuccessOptionalNode)
{
    ///
    ///[b]
    ///  testInt = 10
    ///  testString = ""
    ///
    auto tree = figcone::makeTreeRoot();
    auto& bNode = tree->asItem().addNode("b", {1, 1});
    ;
    bNode.asItem().addParam("testInt", "10", {2, 3});
    bNode.asItem().addParam("testString", "", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<ValidatedOptionalNodeCfg>("", parser);

    ASSERT_EQ(cfg.b.has_value(), true);
    EXPECT_EQ(cfg.b->testInt, 10);
    EXPECT_EQ(cfg.b->testString, "");
}

TEST(StaticReflTestNode, ValidationError)
{
    ///[b]
    ///  testInt = 0
    ///  testString = "":
    ///
    auto tree = figcone::makeTreeRoot();
    auto& bNode = tree->asItem().addNode("b", {1, 1});
    bNode.asItem().addParam("testInt", "0", {2, 3});
    bNode.asItem().addParam("testString", "", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedNodeCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Node 'b': both fields can't be empty");
            });
}

TEST(StaticReflTestNode, ValidationErrorOptionalNode)
{
    ///[b]
    ///  testInt = 0
    ///  testString = "":
    ///
    auto tree = figcone::makeTreeRoot();
    auto& bNode = tree->asItem().addNode("b", {1, 1});
    bNode.asItem().addParam("testInt", "0", {2, 3});
    bNode.asItem().addParam("testString", "", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedOptionalNodeCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Node 'b': both fields can't be empty");
            });
}

TEST(StaticReflTestNode, MultiNodeSingleLevel)
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
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& aNode = tree->asItem().addNode("a", {3, 1});
    aNode.asItem().addParam("testInt", "10", {4, 3});

    auto& bNode = tree->asItem().addNode("b", {5, 1});
    bNode.asItem().addParam("testInt", "11", {6, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<MultiNodeSingleLevelCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.a.testInt, 10);
    ASSERT_TRUE(cfg.b);
    EXPECT_EQ(cfg.b->testInt, 11);
    EXPECT_EQ(cfg.b->testString, "");
}

TEST(StaticReflTestNode, MultiNodeSingleLevelWithoutOptionalNode)
{

    ///foo = 5
    ///bar = test
    ///[a]
    ///  testInt = 10
    ///
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& aNode = tree->asItem().addNode("a", {3, 1});
    aNode.asItem().addParam("testInt", "10", {4, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<MultiNodeSingleLevelCfg>("", parser);

    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.a.testInt, 10);
    ASSERT_FALSE(cfg.b);
}

struct MultiLevelCfg {
    int foo;
    std::string bar;
    B b;
    C c;
};

TEST(StaticReflTestNode, MultiLevel)
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
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& cNode = tree->asItem().addNode("c", {3, 1});
    cNode.asItem().addParam("testInt", "11", {4, 3});
    cNode.asItem().addParam("testDouble", "12", {5, 3});
    auto& bNode = cNode.asItem().addNode("b", {6, 1});
    bNode.asItem().addParam("testInt", "10", {7, 3});
    bNode.asItem().addParam("testString", "Hello world", {8, 3});
    auto& bNode2 = tree->asItem().addNode("b", {10, 1});
    bNode2.asItem().addParam("testInt", "9", {11, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
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

TEST(StaticReflTestNode, MultiLevelAnyNodes)
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
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& cNode = tree->asItem().addAny("c", {3, 1});
    cNode.asItem().addParam("testInt", "11", {4, 3});
    cNode.asItem().addParam("testDouble", "12", {5, 3});
    auto& bNode = cNode.asItem().addAny("b", {6, 1});
    bNode.asItem().addParam("testInt", "10", {7, 3});
    bNode.asItem().addParam("testString", "Hello world", {8, 3});
    auto& bNode2 = tree->asItem().addAny("b", {10, 1});
    bNode2.asItem().addParam("testInt", "9", {11, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
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

TEST(StaticReflTestNode, UnknownNodeError)
{
    ///[test]
    ///  foo = bar
    ///
    auto tree = figcone::makeTreeRoot();
    auto& StaticReflTestNode = tree->asItem().addNode("test", {1, 1});
    StaticReflTestNode.asItem().addParam("foo", "bar", {2, 3});
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<SingleNodeSingleLevelCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Unknown node 'test'");
            });
}

TEST(StaticReflTestNode, NodeListError)
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
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& cNode = tree->asItem().addNode("c", {3, 1});
    cNode.asItem().addParam("testInt", "11", {4, 3});
    cNode.asItem().addParam("testDouble", "12", {5, 3});
    cNode.asItem().addNodeList("b", {6, 1});

    auto& bNode2 = tree->asItem().addNode("b", {10, 1});
    bNode2.asItem().addParam("testInt", "9", {11, 3});

    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<MultiLevelCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:6, column:1] Node 'b': config node can't be a list.");
            });
}

TEST(StaticReflTestNode, AnyNodeSingleElementListNoError)
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
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& cNode = tree->asItem().addNode("c", {3, 1});
    cNode.asItem().addParam("testInt", "11", {4, 3});
    cNode.asItem().addParam("testDouble", "12", {5, 3});
    auto& bListNode = cNode.asItem().addAny("b", {6, 1});
    auto& bListNodeElem = bListNode.asList().emplaceBack({7, 1});
    bListNodeElem.asItem().addParam("testInt", "10", {8, 3});
    bListNodeElem.asItem().addParam("testString", "Hello world", {9, 3});

    auto& bNode2 = tree->asItem().addNode("b", {10, 1});
    bNode2.asItem().addParam("testInt", "9", {11, 3});

    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto parser = TreeProvider{std::move(tree)};

    auto cfg = cfgReader.read<MultiLevelCfg>("", parser);
    EXPECT_EQ(cfg.foo, 5);
    EXPECT_EQ(cfg.bar, "test");
    EXPECT_EQ(cfg.c.testInt, 11);
    EXPECT_EQ(cfg.c.testDouble, 12);
    EXPECT_EQ(cfg.c.b.testInt, 10);
    EXPECT_EQ(cfg.c.b.testString, "Hello world");
    EXPECT_EQ(cfg.b.testInt, 9);
    EXPECT_EQ(cfg.b.testString, "");
}

TEST(StaticReflTestNode, AnyNodeMultiElementListError)
{
    ///foo = 5
    ///bar = test
    ///[c]
    ///  testInt = 11
    ///  testDouble = 12
    ///  [c.b.0]
    ///    testInt = 10
    ///    testString = 'Hello world'
    ///  [c.b.1]
    ///    testInt = 210
    ///    testString = '2Hello world'
    ///
    ///[b]
    ///  testInt = 9
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& cNode = tree->asItem().addNode("c", {3, 1});
    cNode.asItem().addParam("testInt", "11", {4, 3});
    cNode.asItem().addParam("testDouble", "12", {5, 3});
    auto& bListNode = cNode.asItem().addAny("b", {6, 1});
    auto& bListNodeElem = bListNode.asList().emplaceBack({7, 1});
    bListNodeElem.asItem().addParam("testInt", "10", {8, 3});
    bListNodeElem.asItem().addParam("testString", "Hello world", {9, 3});
    auto& bListNodeElem2 = bListNode.asList().emplaceBack({10, 1});
    bListNodeElem2.asItem().addParam("testInt", "210", {11, 3});
    bListNodeElem2.asItem().addParam("testString", "2Hello world", {12, 3});

    auto& bNode2 = tree->asItem().addNode("b", {13, 1});
    bNode2.asItem().addParam("testInt", "9", {14, 3});

    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<MultiLevelCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:6, column:1] Node 'b': config node can't be a list.");
            });
}

TEST(StaticReflTestNode, MissingNodeError)
{
    ///foo = 5
    ///bar = test
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<SingleNodeSingleLevelCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Node 'a' is missing.");
            });
}

TEST(StaticReflTestNode, MissingNodeError2)
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
    tree->asItem().addParam("foo", "5", {1, 1});
    tree->asItem().addParam("bar", "test", {2, 1});
    auto& cNode = tree->asItem().addNode("c", {3, 1});
    cNode.asItem().addParam("testInt", "11", {4, 3});
    cNode.asItem().addParam("testDouble", "12", {5, 3});

    auto& bNode = tree->asItem().addNode("b", {7, 1});
    bNode.asItem().addParam("testInt", "9", {8, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<MultiLevelCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:3, column:1] Node 'c': Node 'b' is missing.");
            });
}

} //namespace test_node
