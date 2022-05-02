#include "assert_exception.h"
#include <gtest/gtest.h>
#include <figcone/config.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif


namespace test_protonodelist {

struct Node : public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_PARAM(testInt, int);
    FIGCONE_PARAM(testStr, std::string);
};

struct Cfg: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_COPY_NODELIST(testNodes, std::vector<Node>);
};

struct Cfg2: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_COPY_NODELIST(testNodes, std::vector<Node>);
    FIGCONE_PARAM(testDouble, double);
};

#ifdef NAMEOF_AVAILABLE
struct CfgWithoutMacro: public figcone::Config<figcone::NameFormat::CamelCase>{
    std::vector<Node> testNodes = copyNodeList<&CfgWithoutMacro::testNodes>();
};
#else
struct CfgWithoutMacro: public figcone::Config<figcone::NameFormat::CamelCase>{
    std::vector<Node> testNodes = copyNodeList<&CfgWithoutMacro::testNodes>("testNodes");
};
#endif

struct NestedCfgList: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_PARAM(testStr, std::string);
    FIGCONE_COPY_NODELIST(testList, std::vector<Cfg2>);
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

TEST(TestCopyNodeList, Basic)
{
    auto cfg = Cfg{};

///[[testNodes]]
///  testInt = 3
///  testStr = Hello
///[[testNodes]]
///  testInt = 2
///[[testNodes]]
///  testStr = World
///[[testNodes]]

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes");
    {
        auto& node = testNodes.asList().addNode();
        node.asItem().addParam("testInt", "3");
        node.asItem().addParam("testStr", "Hello");
    }
    {
        auto& node = testNodes.asList().addNode();
        node.asItem().addParam("testInt", "2");
    }
    {
        auto& node = testNodes.asList().addNode();
        node.asItem().addParam("testStr", "World");
    }
    testNodes.asList().addNode();

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

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

TEST(TestCopyNodeList, BasicWithoutMacro)
{
    auto cfg = CfgWithoutMacro{};

///[[testNodes]]
///  testInt = 3
///  testStr = Hello
///[[testNodes]]
///  testInt = 2
///[[testNodes]]
///  testStr = World
///[[testNodes]]

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes");
    {
        auto& node = testNodes.asList().addNode();
        node.asItem().addParam("testInt", "3");
        node.asItem().addParam("testStr", "Hello");
    }
    {
        auto& node = testNodes.asList().addNode();
        node.asItem().addParam("testInt", "2");
    }
    {
        auto& node = testNodes.asList().addNode();
        node.asItem().addParam("testStr", "World");
    }
    testNodes.asList().addNode();

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

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

TEST(TestCopyNodeList, NestedCfgList)
{
    auto cfg = NestedCfgList{};

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
    tree.asItem().addParam("testStr", "Root");
    auto& testList = tree.asItem().addNodeList("testList");
    {
        auto& node = testList.asList().addNode();
        node.asItem().addParam("testDouble", "0.12");
        auto& testNodes = node.asItem().addNodeList("testNodes");
        {
            auto& newNode = testNodes.asList().addNode();
            newNode.asItem().addParam("testInt", "3");
            newNode.asItem().addParam("testStr", "Foo");
        }
        {
            testNodes.asList().addNode();
        }
    }
    {
        auto& node = testList.asList().addNode();
        auto& testNodes = node.asItem().addNodeList("testNodes");
        {
            auto& newNode = testNodes.asList().addNode();
            newNode.asItem().addParam("testInt", "5");
            newNode.asItem().addParam("testStr", "Bar");
        }
        {
            auto& newNode = testNodes.asList().addNode();
            newNode.asItem().addParam("testInt", "6");
        }
    }
    {
        auto& node = testList.asList().addNode();
        node.asItem().addParam("testDouble", "0.33");
    }

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

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

TEST(TestCopyNodeList, MissingNodeListError)
{
    auto cfg = Cfg{};

///testStr = Hello
///
    auto tree = figcone::makeTreeRoot();
    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Node 'testNodes' is missing.");
    });
}

TEST(TestNodeList, InvalidListElementError)
{
    auto cfg = Cfg{};

///[[testNodes]]
///    testInt = error
///[[testNodes]]
///    testInt = 2

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes", {1, 1});
    {
        auto& node = testNodes.asList().addNode({1, 1});
        node.asItem().addParam("testInt", "error", {2, 3});
    }
    {
        auto& node = testNodes.asList().addNode({3, 1});
        node.asItem().addParam("testInt", "2", {4, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&]{
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:2, column:3] Couldn't set parameter 'testInt' value from 'error'");
    });
}

TEST(TestNodeList, IncompleteListElementError)
{
    auto cfg = Cfg{};

///[[testNodes]]
///[[testNodes]]
///    testInt = 2


    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes", {1, 1});
    testNodes.asList().addNode({1, 1});
    {
        auto& node = testNodes.asList().addNode({2, 1});
        node.asItem().addParam("testInt", "2", {3, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&]{
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Node list 'testNodes': Parameter 'testInt' is missing.");
    });
}


}
