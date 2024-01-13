#include "assert_exception.h"
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <gtest/gtest.h>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif

namespace test_copynodelist {

struct Node {
    int testInt;
    std::string testStr;
};

struct Cfg {
    std::vector<Node> testNodes;

    using traits = figcone::FieldTraits< //
            figcone::CopyNodeListField<&Cfg::testNodes>>;
};

struct Cfg2 {
    std::vector<Node> testNodes;
    double testDouble;

    using traits = figcone::FieldTraits< //
            figcone::CopyNodeListField<&Cfg2::testNodes>>;
};

struct NestedCfgList {
    std::string testStr;
    std::vector<Cfg2> testList;

    using traits = figcone::FieldTraits< //
            figcone::CopyNodeListField<&NestedCfgList::testList>>;
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

TEST(StaticReflTestCopyNodeList, Basic)
{
    ///[[testNodes]]
    ///  testInt = 3
    ///  testStr = Hello
    ///[[testNodes]]
    ///  testInt = 2
    ///[[testNodes]]
    ///  testStr = World
    ///[[testNodes]]

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree->asItem().addNodeList("testNodes");
    {
        auto& node = testNodes.asList().emplaceBack();
        node.asItem().addParam("testInt", "3");
        node.asItem().addParam("testStr", "Hello");
    }
    {
        auto& node = testNodes.asList().emplaceBack();
        node.asItem().addParam("testInt", "2");
    }
    {
        auto& node = testNodes.asList().emplaceBack();
        node.asItem().addParam("testStr", "World");
    }
    testNodes.asList().emplaceBack();

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<Cfg>("", parser);

    ASSERT_EQ(cfg.testNodes.size(), 4);
    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
    EXPECT_EQ(cfg.testNodes[0].testStr, "Hello");
    EXPECT_EQ(cfg.testNodes[1].testInt, 2);
    EXPECT_EQ(cfg.testNodes[1].testStr, "Hello");
    EXPECT_EQ(cfg.testNodes[2].testInt, 3);
    EXPECT_EQ(cfg.testNodes[2].testStr, "World");
    EXPECT_EQ(cfg.testNodes[3].testInt, 3);
    EXPECT_EQ(cfg.testNodes[3].testStr, "Hello");
}

TEST(StaticReflTestCopyNodeList, NestedCfgList)
{
    ///testStr = Root
    ///[[testList]]
    ///  testDouble = 0.12
    ///  [[testList.testNodes]]
    ///    testInt = 3
    ///    testStr = Foo
    ///  [[testList.testNodes]]
    ///[[testList]]
    ///  [[testList.testNodes]]
    ///    testInt = 5
    ///    testStr = Bar
    ///  [[testList.testNodes]]
    ///[[testList]]
    ///  testDouble = 0.33

    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("testStr", "Root");
    auto& testList = tree->asItem().addNodeList("testList");
    {
        auto& node = testList.asList().emplaceBack();
        node.asItem().addParam("testDouble", "0.12");
        auto& testNodes = node.asItem().addNodeList("testNodes");
        {
            auto& newNode = testNodes.asList().emplaceBack();
            newNode.asItem().addParam("testInt", "3");
            newNode.asItem().addParam("testStr", "Foo");
        }
        {
            testNodes.asList().emplaceBack();
        }
    }
    {
        auto& node = testList.asList().emplaceBack();
        auto& testNodes = node.asItem().addNodeList("testNodes");
        {
            auto& newNode = testNodes.asList().emplaceBack();
            newNode.asItem().addParam("testInt", "5");
            newNode.asItem().addParam("testStr", "Bar");
        }
        {
            auto& newNode = testNodes.asList().emplaceBack();
            newNode.asItem().addParam("testInt", "6");
        }
    }
    {
        auto& node = testList.asList().emplaceBack();
        node.asItem().addParam("testDouble", "0.33");
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<NestedCfgList>("", parser);

    EXPECT_EQ(cfg.testStr, "Root");
    ASSERT_EQ(cfg.testList.size(), 3);
    EXPECT_EQ(cfg.testList.at(0).testDouble, 0.12);
    ASSERT_EQ(cfg.testList.at(0).testNodes.size(), 2);
    EXPECT_EQ(cfg.testList.at(0).testNodes.at(0).testInt, 3);
    EXPECT_EQ(cfg.testList.at(0).testNodes.at(0).testStr, "Foo");
    EXPECT_EQ(cfg.testList.at(0).testNodes.at(1).testInt, 3);
    EXPECT_EQ(cfg.testList.at(0).testNodes.at(1).testStr, "Foo");
    EXPECT_EQ(cfg.testList.at(1).testDouble, 0.12);
    ASSERT_EQ(cfg.testList.at(1).testNodes.size(), 2);
    EXPECT_EQ(cfg.testList.at(1).testNodes.at(0).testInt, 5);
    EXPECT_EQ(cfg.testList.at(1).testNodes.at(0).testStr, "Bar");
    EXPECT_EQ(cfg.testList.at(1).testNodes.at(1).testInt, 6);
    EXPECT_EQ(cfg.testList.at(1).testNodes.at(1).testStr, "Bar");
    EXPECT_EQ(cfg.testList.at(2).testDouble, 0.33);
    ASSERT_EQ(cfg.testList.at(2).testNodes.size(), 2);
    EXPECT_EQ(cfg.testList.at(2).testNodes.at(0).testInt, 3);
    EXPECT_EQ(cfg.testList.at(2).testNodes.at(0).testStr, "Foo");
    EXPECT_EQ(cfg.testList.at(2).testNodes.at(1).testInt, 3);
    EXPECT_EQ(cfg.testList.at(2).testNodes.at(1).testStr, "Foo");
}

TEST(StaticReflTestCopyNodeList, MissingNodeListError)
{
    ///testStr = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<Cfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Node 'testNodes' is missing.");
            });
}

TEST(TestNodeList, InvalidListElementError)
{
    ///[[testNodes]]
    ///    testInt = error
    ///[[testNodes]]
    ///    testInt = 2

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree->asItem().addNodeList("testNodes", {1, 1});
    {
        auto& node = testNodes.asList().emplaceBack({1, 1});
        node.asItem().addParam("testInt", "error", {2, 3});
    }
    {
        auto& node = testNodes.asList().emplaceBack({3, 1});
        node.asItem().addParam("testInt", "2", {4, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<Cfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:2, column:3] Couldn't set parameter 'testInt' value from 'error'");
            });
}

TEST(TestNodeList, IncompleteListElementError)
{
    ///[[testNodes]]
    ///[[testNodes]]
    ///    testInt = 2

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree->asItem().addNodeList("testNodes", {1, 1});
    testNodes.asList().emplaceBack({1, 1});
    {
        auto& node = testNodes.asList().emplaceBack({2, 1});
        node.asItem().addParam("testInt", "2", {3, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<Cfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Node list 'testNodes': Parameter 'testInt' is missing.");
            });
}

} //namespace test_copynodelist
