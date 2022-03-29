#include "assert_exception.h"
#include <gtest/gtest.h>
#include <figcone/config.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>

namespace test_nodelist {

struct Node : public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_PARAM(testInt, int);
};

struct Cfg: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_NODELIST(testNodes, Node);
    FIGCONE_PARAM(testStr, std::string);
};

struct Cfg2: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_PARAM(testDouble, double);
    FIGCONE_NODE(testNode, Node);
};

struct MinSizeListValidator{
public:
    MinSizeListValidator(int minSize)
        : minSize_{minSize}
    {}

    void operator()(const std::vector<Node>& nodeList)
    {
        if (nodeList.size() < minSize_) throw figcone::ValidationError{"can't have less than 2 elements"};
    }
private:
    int minSize_;
};

struct ValidatedCfg: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_NODELIST(testNodes, Node).checkedWith([](const std::vector<Node>& nodeList){
        if (nodeList.size() < 2) throw figcone::ValidationError{"can't have less than 2 elements"};
    });
};

struct ValidatedWithFunctorCfg: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_NODELIST(testNodes, Node).checkedWith<MinSizeListValidator>(2);
};

struct CfgWithoutMacro: public figcone::Config<figcone::NameFormat::CamelCase>{
    std::vector<Node> testNodes = nodeList(&CfgWithoutMacro::testNodes, "testNodes");
};

struct NestedCfg: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_PARAM(testDouble, double);
    FIGCONE_NODE(testCfg, Cfg);
};

struct NestedCfgList: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_PARAM(testStr, std::string);
    FIGCONE_NODELIST(testList, Cfg);
};

struct NestedCfg2List: public figcone::Config<figcone::NameFormat::CamelCase>{
    FIGCONE_PARAM(testStr, std::string);
    FIGCONE_NODELIST(testList, Cfg2);
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

TEST(TestNodeList, Basic)
{
    auto cfg = Cfg{};

///testStr = Hello
///[[testNodes]]
///  testInt = 3
///[[testNodes]]
///  testInt = 2
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testStr", "Hello", {1,1});
    auto& testNodes = tree.asItem().addNodeList("testNodes", {2, 1});
    {
        auto& node = testNodes.asList().addNode({2, 1});
        node.asItem().addParam("testInt", "3", {3, 3});
    }
    {
        auto& node = testNodes.asList().addNode({4, 1});
        node.asItem().addParam("testInt", "2", {5, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    ASSERT_EQ(cfg.testNodes.size(), 2);
    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
    EXPECT_EQ(cfg.testNodes[1].testInt, 2);
    EXPECT_EQ(cfg.testStr, "Hello");
}

TEST(TestNodeList, BasicWithoutMacro)
{
    auto cfg = CfgWithoutMacro{};

///[[testNodes]]
///  testInt = 3
///[[testNodes]]
///  testInt = 2
///
    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes", {1, 1});
    {
        auto& node = testNodes.asList().addNode({2, 1});
        node.asItem().addParam("testInt", "3", {2, 3});
    }
    {
        auto& node = testNodes.asList().addNode({3, 1});
        node.asItem().addParam("testInt", "2", {4, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    ASSERT_EQ(cfg.testNodes.size(), 2);
    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
    EXPECT_EQ(cfg.testNodes[1].testInt, 2);
}

TEST(TestNodeList, ValidationSuccess)
{
    auto cfg = ValidatedCfg{};

///[[testNodes]]
///  testInt = 3
///[[testNodes]]
///  testInt = 4
///
    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes", {1, 1});
    {
        auto& node = testNodes.asList().addNode({2, 1});
        node.asItem().addParam("testInt", "3", {2, 3});
    }
    {
        auto& node = testNodes.asList().addNode({3, 1});
        node.asItem().addParam("testInt", "4", {4, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    ASSERT_EQ(cfg.testNodes.size(), 2);
    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
    EXPECT_EQ(cfg.testNodes[1].testInt, 4);
}

TEST(TestNodeList, ValidationFailure)
{
    auto cfg = ValidatedCfg{};

///[[testNodes]]
///  testInt = 3
///
    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes", {1, 1});
    {
        auto& node = testNodes.asList().addNode({2, 1});
        node.asItem().addParam("testInt", "3", {2, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Node list 'testNodes': can't have less than 2 elements");
    });
}

TEST(TestNodeList, ValidationWithFunctorFailure)
{
    auto cfg = ValidatedWithFunctorCfg{};

///[[testNodes]]
///  testInt = 3
///
    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes", {1, 1});
    {
        auto& node = testNodes.asList().addNode({2, 1});
        node.asItem().addParam("testInt", "3", {2, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&] {
        cfg.read(R"(
        #testNodes:
        ###
            testInt = 3
    )", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Node list 'testNodes': can't have less than 2 elements");
    });
}

TEST(TestNodeList, NestedCfgList)
{
    auto cfg = NestedCfgList{};

///testStr = Hello
///[[testList]]
///  testStr = Hello
///  [[testList.testNodes]]
///    testInt = 3
///  [[testList.testNodes]]
///    testInt = 33
///[[testList]]
///  testStr = World
///  [[testList.testNodes]]
///    testInt = 5
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testStr", "Hello", {1, 1});
    auto& testList = tree.asItem().addNodeList("testList", {2, 1});
    {
        auto& node = testList.asList().addNode({2, 1});
        node.asItem().addParam("testStr", "Hello", {3, 3});
        auto& testNodes = node.asItem().addNodeList("testNodes", {4, 3});
        {
            auto& newNode = testNodes.asList().addNode({4, 3});
            newNode.asItem().addParam("testInt", "3", {5, 3});
        }
        {
            auto& newNode = testNodes.asList().addNode({6, 3});
            newNode.asItem().addParam("testInt", "33", {7, 3});
        }
    }
    {
        auto& node = testList.asList().addNode({8, 1});
        node.asItem().addParam("testStr", "World", {8, 3});
        auto& testNodes = node.asItem().addNodeList("testNodes", {9, 3});
        {
            auto& newNode = testNodes.asList().addNode({9, 3});
            newNode.asItem().addParam("testInt", "5", {10, 5});
        }
    }

    auto parser = TreeProvider{std::move(tree)};
    cfg.read("", parser);

    EXPECT_EQ(cfg.testStr, "Hello");
    ASSERT_EQ(cfg.testList.size(), 2);
    EXPECT_EQ(cfg.testList.at(0).testStr, "Hello");
    ASSERT_EQ(cfg.testList.at(0).testNodes.size(), 2);
    EXPECT_EQ(cfg.testList.at(0).testNodes.at(0).testInt, 3);
    EXPECT_EQ(cfg.testList.at(0).testNodes.at(1).testInt, 33);
    EXPECT_EQ(cfg.testList.at(1).testStr, "World");
    ASSERT_EQ(cfg.testList.at(1).testNodes.size(), 1);
    EXPECT_EQ(cfg.testList.at(1).testNodes.at(0).testInt, 5);
}

TEST(TestNodeList, MissingNodeListError)
{
    auto cfg = Cfg{};

///testStr = Hello
///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testStr", "Hello", {1, 1});

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

///testStr = Hello
///[[testNodes]]
///    testInt = error
///[[testNodes]]
///    testInt = 2

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes", {2, 1});
    {
        auto& node = testNodes.asList().addNode({2, 1});
        node.asItem().addParam("testInt", "error", {3, 3});
    }
    {
        auto& node = testNodes.asList().addNode({4, 1});
        node.asItem().addParam("testInt", "2", {5, 3});
    }
    tree.asItem().addParam("testStr", "Hello", {1, 1});

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&]{
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:3, column:3] Couldn't set parameter 'testInt' value from 'error'");
    });
}

TEST(TestNodeList, IncompleteListElementError)
{
    auto cfg = Cfg{};

///testStr = Hello
///[[testNodes]]
///[[testNodes]]
///    testInt = 2


    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree.asItem().addNodeList("testNodes", {2, 1});
    testNodes.asList().addNode({2, 1});
    {
        auto& node = testNodes.asList().addNode({3, 1});
        node.asItem().addParam("testInt", "2", {4, 3});
    }
    tree.asItem().addParam("testStr", "Hello", {1, 1});

    auto parser = TreeProvider{std::move(tree)};
    assert_exception<figcone::ConfigError>([&]{
        cfg.read("", parser);
    }, [](const figcone::ConfigError& error){
        EXPECT_EQ(std::string{error.what()}, "[line:2, column:1] Node list 'testNodes': Parameter 'testInt' is missing.");
    });
}


}
