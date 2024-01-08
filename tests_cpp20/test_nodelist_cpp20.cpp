#include "assert_exception.h"
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <gtest/gtest.h>

#include <deque>
#include <list>

#if __has_include(<nameof.hpp>)
#define NAMEOF_AVAILABLE
#endif

namespace test_nodelist {

struct Node {
    int testInt;
};

struct Cfg {
    std::vector<Node> testNodes;
    std::list<Node> optTestNodes;
    std::optional<std::deque<Node>> optTestNodes2;
    std::string testStr;

    using traits = figcone::FieldTraits< //
            figcone::OptionalField<&Cfg::optTestNodes>>;
};

struct Cfg2 {
    double testDouble;
    Node testNode;
};

template<int minSize>
struct NotLessThan {
public:
    void operator()(const std::vector<Node>& nodeList)
    {
        if (static_cast<int>(nodeList.size()) < minSize)
            throw figcone::ValidationError{"can't have less than 2 elements"};
    }
};

struct ValidatedCfg {
    std::vector<Node> testNodes;

    using traits = figcone::FieldTraits<figcone::ValidatedField<&ValidatedCfg::testNodes, NotLessThan<2>>>;
};

struct ValidatedOptionalNodeListCfg {
    figcone::optional<std::vector<Node>> testNodesOpt;

    using traits =
            figcone::FieldTraits<figcone::ValidatedField<&ValidatedOptionalNodeListCfg::testNodesOpt, NotLessThan<2>>>;
};

struct NestedCfgList {
    std::string testStr;
    std::deque<Cfg> testList;
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

TEST(StaticReflTestNodeList, Basic)
{
    ///testStr = Hello
    ///[[testNodes]]
    ///  testInt = 3
    ///[[testNodes]]
    ///  testInt = 2
    ///[[optTestNodes]]
    ///   testInt = 100
    ///[[optTestNodes2]]
    ///   testInt = 200
    ///[[optTestNodes2]]
    ///   testInt = 300
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("testStr", "Hello", {1, 1});
    auto& testNodes = tree->asItem().addNodeList("testNodes", {2, 1});
    {
        auto& node = testNodes.asList().emplaceBack({2, 1});
        node.asItem().addParam("testInt", "3", {3, 3});
    }
    {
        auto& node = testNodes.asList().emplaceBack({4, 1});
        node.asItem().addParam("testInt", "2", {5, 3});
    }
    auto& optTestNodes = tree->asItem().addNodeList("optTestNodes", {6, 1});
    {
        auto& node = optTestNodes.asList().emplaceBack({6, 1});
        node.asItem().addParam("testInt", "100", {7, 3});
    }
    auto& optTestNodes2 = tree->asItem().addNodeList("optTestNodes2", {8, 1});
    {
        {
            auto& node = optTestNodes2.asList().emplaceBack({8, 1});
            node.asItem().addParam("testInt", "200", {9, 3});
        }
        {
            auto& node = optTestNodes2.asList().emplaceBack({10, 1});
            node.asItem().addParam("testInt", "300", {11, 3});
        }
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<Cfg>("", parser);

    ASSERT_EQ(cfg.testNodes.size(), 2);
    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
    EXPECT_EQ(cfg.testNodes[1].testInt, 2);
    ASSERT_EQ(cfg.optTestNodes.size(), 1);
    EXPECT_EQ(cfg.optTestNodes.front().testInt, 100);
    ASSERT_TRUE(cfg.optTestNodes2);
    ASSERT_EQ(cfg.optTestNodes2->size(), 2);
    ASSERT_EQ(cfg.optTestNodes2->at(0).testInt, 200);
    ASSERT_EQ(cfg.optTestNodes2->at(1).testInt, 300);
    EXPECT_EQ(cfg.testStr, "Hello");
}

TEST(StaticReflTestNodeList, BasicAnyNode)
{
    ///testStr = Hello
    ///[[testNodes]]
    ///  testInt = 3
    ///[[testNodes]]
    ///  testInt = 2
    ///[[optTestNodes]]
    ///   testInt = 100
    ///[[optTestNodes2]]
    ///   testInt = 200
    ///[[optTestNodes2]]
    ///   testInt = 300
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("testStr", "Hello", {1, 1});
    auto& testNodes = tree->asItem().addAny("testNodes", {2, 1});
    {
        auto& node = testNodes.asList().emplaceBack({2, 1});
        node.asItem().addParam("testInt", "3", {3, 3});
    }
    {
        auto& node = testNodes.asList().emplaceBack({4, 1});
        node.asItem().addParam("testInt", "2", {5, 3});
    }
    auto& optTestNodes = tree->asItem().addNodeList("optTestNodes", {6, 1});
    {
        auto& node = optTestNodes.asList().emplaceBackAny({6, 1});
        node.asItem().addParam("testInt", "100", {7, 3});
    }
    auto& optTestNodes2 = tree->asItem().addAny("optTestNodes2", {8, 1});
    {
        {
            auto& node = optTestNodes2.asList().emplaceBackAny({8, 1});
            node.asItem().addParam("testInt", "200", {9, 3});
        }
        {
            auto& node = optTestNodes2.asList().emplaceBackAny({10, 1});
            node.asItem().addParam("testInt", "300", {11, 3});
        }
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<Cfg>("", parser);

    ASSERT_EQ(cfg.testNodes.size(), 2);
    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
    EXPECT_EQ(cfg.testNodes[1].testInt, 2);
    ASSERT_EQ(cfg.optTestNodes.size(), 1);
    ASSERT_EQ(cfg.optTestNodes.size(), 1);
    EXPECT_EQ(cfg.optTestNodes.front().testInt, 100);
    ASSERT_TRUE(cfg.optTestNodes2);
    ASSERT_EQ(cfg.optTestNodes2->size(), 2);
    ASSERT_EQ(cfg.optTestNodes2->at(0).testInt, 200);
    ASSERT_EQ(cfg.optTestNodes2->at(1).testInt, 300);
    EXPECT_EQ(cfg.testStr, "Hello");
}
//
//TEST(StaticReflTestNodeList, BasicNonAggregate)
//{
//    ///testStr = Hello
//    ///[[testNodes]]
//    ///  testInt = 3
//    ///[[testNodes]]
//    ///  testInt = 2
//    ///[[optTestNodes]]
//    ///   testInt = 100
//    ///[[optTestNodes2]]
//    ///   testInt = 200
//    ///[[optTestNodes2]]
//    ///   testInt = 300
//    auto tree = figcone::makeTreeRoot();
//    tree->asItem().addParam("testStr", "Hello", {1, 1});
//    auto& testNodes = tree->asItem().addNodeList("testNodes", {2, 1});
//    {
//        auto& node = testNodes.asList().emplaceBack({2, 1});
//        node.asItem().addParam("testInt", "3", {3, 3});
//    }
//    {
//        auto& node = testNodes.asList().emplaceBack({4, 1});
//        node.asItem().addParam("testInt", "2", {5, 3});
//    }
//    auto& optTestNodes = tree->asItem().addNodeList("optTestNodes", {6, 1});
//    {
//        auto& node = optTestNodes.asList().emplaceBack({6, 1});
//        node.asItem().addParam("testInt", "100", {7, 3});
//    }
//    auto& optTestNodes2 = tree->asItem().addNodeList("optTestNodes2", {8, 1});
//    {
//        {
//            auto& node = optTestNodes2.asList().emplaceBack({8, 1});
//            node.asItem().addParam("testInt", "200", {9, 3});
//        }
//        {
//            auto& node = optTestNodes2.asList().emplaceBack({10, 1});
//            node.asItem().addParam("testInt", "300", {11, 3});
//        }
//    }
//
//    auto parser = TreeProvider{std::move(tree)};
//    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
//    auto cfg = cfgReader.read<NonAggregateCfg>("", parser);
//
//    ASSERT_EQ(cfg.testNodes.size(), 2);
//    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
//    EXPECT_EQ(cfg.testNodes[1].testInt, 2);
//    ASSERT_EQ(cfg.optTestNodes.size(), 1);
//    EXPECT_EQ(cfg.optTestNodes.front().testInt, 100);
//    ASSERT_TRUE(cfg.optTestNodes2);
//    ASSERT_EQ(cfg.optTestNodes2->size(), 2);
//    ASSERT_EQ(cfg.optTestNodes2->at(0).testInt, 200);
//    ASSERT_EQ(cfg.optTestNodes2->at(1).testInt, 300);
//    EXPECT_EQ(cfg.testStr, "Hello");
//}

TEST(StaticReflTestNodeList, BasicAnyWithoutOptional)
{

    ///testStr = Hello
    ///[[testNodes]]
    ///  testInt = 3
    ///[[testNodes]]
    ///  testInt = 2
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("testStr", "Hello", {1, 1});
    auto& testNodes = tree->asItem().addNodeList("testNodes", {2, 1});
    {
        auto& node = testNodes.asList().emplaceBack({2, 1});
        node.asItem().addParam("testInt", "3", {3, 3});
    }
    {
        auto& node = testNodes.asList().emplaceBack({4, 1});
        node.asItem().addParam("testInt", "2", {5, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<Cfg>("", parser);

    ASSERT_EQ(cfg.testNodes.size(), 2);
    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
    EXPECT_EQ(cfg.testNodes[1].testInt, 2);
    ASSERT_EQ(cfg.optTestNodes.size(), 0);
    ASSERT_FALSE(cfg.optTestNodes2);
    EXPECT_EQ(cfg.testStr, "Hello");
}
//
//TEST(StaticReflTestNodeList, BasicWithoutMacro)
//{
//    ///[[testNodes]]
//    ///  testInt = 3
//    ///[[testNodes]]
//    ///  testInt = 2
//    ///
//    auto tree = figcone::makeTreeRoot();
//    auto& testNodes = tree->asItem().addNodeList("testNodes", {1, 1});
//    {
//        auto& node = testNodes.asList().emplaceBack({2, 1});
//        node.asItem().addParam("testInt", "3", {2, 3});
//    }
//    {
//        auto& node = testNodes.asList().emplaceBack({3, 1});
//        node.asItem().addParam("testInt", "2", {4, 3});
//    }
//
//    auto parser = TreeProvider{std::move(tree)};
//    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
//    auto cfg = cfgReader.read<CfgWithoutMacro>("", parser);
//
//    ASSERT_EQ(cfg.testNodes.size(), 2);
//    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
//    EXPECT_EQ(cfg.testNodes[1].testInt, 2);
//}
//
TEST(StaticReflTestNodeList, ValidationSuccess)
{
    ///[[testNodes]]
    ///  testInt = 3
    ///[[testNodes]]
    ///  testInt = 4

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree->asItem().addNodeList("testNodes", {1, 1});
    {
        auto& node = testNodes.asList().emplaceBack({2, 1});
        node.asItem().addParam("testInt", "3", {2, 3});
    }
    {
        auto& node = testNodes.asList().emplaceBack({3, 1});
        node.asItem().addParam("testInt", "4", {4, 3});
    }
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<ValidatedCfg>("", parser);

    ASSERT_EQ(cfg.testNodes.size(), 2);
    EXPECT_EQ(cfg.testNodes[0].testInt, 3);
    EXPECT_EQ(cfg.testNodes[1].testInt, 4);
}

TEST(StaticReflTestNodeList, ValidationSuccessOptionalNodeList)
{
    ///[[testNodesOpt]]
    ///  testInt = 5
    ///[[testNodesOpt]]
    ///  testInt = 6
    auto tree = figcone::makeTreeRoot();
    auto& testNodesOpt = tree->asItem().addNodeList("testNodesOpt", {5, 1});
    {
        auto& node = testNodesOpt.asList().emplaceBack({5, 1});
        node.asItem().addParam("testInt", "5", {6, 3});
    }
    {
        auto& node = testNodesOpt.asList().emplaceBack({7, 1});
        node.asItem().addParam("testInt", "6", {8, 3});
    }
    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<ValidatedOptionalNodeListCfg>("", parser);
    ASSERT_TRUE(cfg.testNodesOpt.has_value());
    ASSERT_EQ(cfg.testNodesOpt->size(), 2);
    EXPECT_EQ(cfg.testNodesOpt->at(0).testInt, 5);
    EXPECT_EQ(cfg.testNodesOpt->at(1).testInt, 6);
}

TEST(StaticReflTestNodeList, ValidationFailure)
{
    ///[[testNodes]]
    ///  testInt = 3
    ///
    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree->asItem().addNodeList("testNodes", {1, 1});
    {
        auto& node = testNodes.asList().emplaceBack({2, 1});
        node.asItem().addParam("testInt", "3", {2, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Node list 'testNodes': can't have less than 2 elements");
            });
}

TEST(StaticReflTestNodeList, ValidationFailureOptionalNodeList)
{
    ///[[testNodes]]
    ///  testInt = 3
    ///
    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree->asItem().addNodeList("testNodesOpt", {1, 1});
    {
        auto& node = testNodes.asList().emplaceBack({2, 1});
        node.asItem().addParam("testInt", "3", {2, 3});
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                cfgReader.read<ValidatedOptionalNodeListCfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(
                        std::string{error.what()},
                        "[line:1, column:1] Node list 'testNodesOpt': can't have less than 2 elements");
            });
}

TEST(StaticReflTestNodeList, NestedCfgList)
{
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
    tree->asItem().addParam("testStr", "Hello", {1, 1});
    auto& testList = tree->asItem().addNodeList("testList", {2, 1});
    {
        auto& node = testList.asList().emplaceBack({2, 1});
        node.asItem().addParam("testStr", "Hello", {3, 3});
        auto& testNodes = node.asItem().addNodeList("testNodes", {4, 3});
        {
            auto& newNode = testNodes.asList().emplaceBack({4, 3});
            newNode.asItem().addParam("testInt", "3", {5, 3});
        }
        {
            auto& newNode = testNodes.asList().emplaceBack({6, 3});
            newNode.asItem().addParam("testInt", "33", {7, 3});
        }
    }
    {
        auto& node = testList.asList().emplaceBack({8, 1});
        node.asItem().addParam("testStr", "World", {8, 3});
        auto& testNodes = node.asItem().addNodeList("testNodes", {9, 3});
        {
            auto& newNode = testNodes.asList().emplaceBack({9, 3});
            newNode.asItem().addParam("testInt", "5", {10, 5});
        }
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<NestedCfgList>("", parser);

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

TEST(StaticReflTestNodeList, NestedCfgAnyList)
{
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
    tree->asItem().addParam("testStr", "Hello", {1, 1});
    auto& testList = tree->asItem().addAny("testList", {2, 1});
    {
        auto& node = testList.asList().emplaceBackAny({2, 1});
        node.asItem().addParam("testStr", "Hello", {3, 3});
        auto& testNodes = node.asItem().addAny("testNodes", {4, 3});
        {
            auto& newNode = testNodes.asList().emplaceBackAny({4, 3});
            newNode.asItem().addParam("testInt", "3", {5, 3});
        }
        {
            auto& newNode = testNodes.asList().emplaceBackAny({6, 3});
            newNode.asItem().addParam("testInt", "33", {7, 3});
        }
    }
    {
        auto& node = testList.asList().emplaceBackAny({8, 1});
        node.asItem().addParam("testStr", "World", {8, 3});
        auto& testNodes = node.asItem().addAny("testNodes", {9, 3});
        {
            auto& newNode = testNodes.asList().emplaceBackAny({9, 3});
            newNode.asItem().addParam("testInt", "5", {10, 5});
        }
    }

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<NestedCfgList>("", parser);

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

TEST(StaticReflTestNodeList, MissingNodeListError)
{
    ///testStr = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("testStr", "Hello", {1, 1});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    assert_exception<figcone::ConfigError>(
            [&]
            {
                auto cfg = cfgReader.read<Cfg>("", parser);
            },
            [](const figcone::ConfigError& error)
            {
                EXPECT_EQ(std::string{error.what()}, "[line:1, column:1] Root node: Node 'testNodes' is missing.");
            });
}

TEST(StaticReflTestNodeList, NodeNotListError)
{
    ///testStr = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree->asItem().addParam("testStr", "Hello", {1, 1});
    tree->asItem().addNode("testNodes", {2, 1});

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
                        "[line:2, column:1] Node list 'testNodes': config node must be a list.");
            });
}

TEST(StaticReflTestNodeList, InvalidListElementError)
{
    ///testStr = Hello
    ///[[testNodes]]
    ///    testInt = error
    ///[[testNodes]]
    ///    testInt = 2

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree->asItem().addNodeList("testNodes", {2, 1});
    {
        auto& node = testNodes.asList().emplaceBack({2, 1});
        node.asItem().addParam("testInt", "error", {3, 3});
    }
    {
        auto& node = testNodes.asList().emplaceBack({4, 1});
        node.asItem().addParam("testInt", "2", {5, 3});
    }
    tree->asItem().addParam("testStr", "Hello", {1, 1});

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
                        "[line:3, column:3] Couldn't set parameter 'testInt' value from 'error'");
            });
}

TEST(StaticReflTestNodeList, IncompleteListElementError)
{
    ///testStr = Hello
    ///[[testNodes]]
    ///[[testNodes]]
    ///    testInt = 2

    auto tree = figcone::makeTreeRoot();
    auto& testNodes = tree->asItem().addNodeList("testNodes", {2, 1});
    testNodes.asList().emplaceBack({2, 1});
    {
        auto& node = testNodes.asList().emplaceBack({3, 1});
        node.asItem().addParam("testInt", "2", {4, 3});
    }
    tree->asItem().addParam("testStr", "Hello", {1, 1});

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
                        "[line:2, column:1] Node list 'testNodes': Parameter 'testInt' is missing.");
            });
}

} //namespace test_nodelist
