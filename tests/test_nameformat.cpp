#include "assert_exception.h"
#include <figcone/config.h>
#include <figcone/configreader.h>
#include <figcone/errors.h>
#include <figcone_tree/tree.h>
#include <gtest/gtest.h>

namespace test_nameformat {

struct OriginalNamesInnerCfg : public figcone::Config {
    FIGCONE_PARAM(testStr, std::string);
};

struct OriginalNamesCfg : public figcone::Config {
    FIGCONE_PARAM(test_int_, int);
    FIGCONE_NODE(test_Inner, OriginalNamesInnerCfg);
};

class TreeProvider : public figcone::IParser {
public:
    TreeProvider(figcone::TreeNode tree)
        : tree_(std::move(tree))
    {
    }

    figcone::TreeNode parse(std::istream&) override
    {
        return std::move(tree_);
    }

    figcone::TreeNode tree_;
};

TEST(NameFormat, OriginalNames)
{
    ///test_int = 10
    ///#test_Inner:
    ///  testStr = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test_int", "10", {1, 1});
    auto& node = tree.asItem().addNode("test_Inner", {2, 1});
    node.asItem().addParam("testStr", "Hello", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::Original};

    auto cfg = cfgReader.read<OriginalNamesCfg>("", parser);

    EXPECT_EQ(cfg.test_int_, 10);
    EXPECT_EQ(cfg.test_Inner.testStr, "Hello");
}

struct InnerSnakeStructCamelCfg : public figcone::Config {
    FIGCONE_PARAM(test_str, std::string);
};

struct SnakeStructCamelCfg : public figcone::Config {
    FIGCONE_PARAM(test_int, int);
    FIGCONE_NODE(test_inner, InnerSnakeStructCamelCfg);
};

TEST(NameFormat, SnakeStructCamelCfg)
{
    ///testInt = 10
    ///#testInner:
    ///  testStr = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testInt", "10", {1, 1});
    auto& node = tree.asItem().addNode("testInner", {2, 1});
    node.asItem().addParam("testStr", "Hello", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<SnakeStructCamelCfg>("", parser);
    EXPECT_EQ(cfg.test_int, 10);
    EXPECT_EQ(cfg.test_inner.test_str, "Hello");
}

struct InnerSnakeStructSnakeCfg : public figcone::Config {
    FIGCONE_PARAM(test_str, std::string);
};

struct SnakeStructSnakeCfg : public figcone::Config {
    FIGCONE_PARAM(test_int, int);
    FIGCONE_NODE(test_inner, InnerSnakeStructSnakeCfg);
};

TEST(NameFormat, SnakeStructSnakeCfg)
{
    ///test_int = 10
    ///#test_inner:
    ///  test_str = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test_int", "10", {1, 1});
    auto& node = tree.asItem().addNode("test_inner", {2, 1});
    node.asItem().addParam("test_str", "Hello", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::SnakeCase};
    auto cfg = cfgReader.read<SnakeStructSnakeCfg>("", parser);
    EXPECT_EQ(cfg.test_int, 10);
    EXPECT_EQ(cfg.test_inner.test_str, "Hello");
}

struct InnerSnakeStructKebabCfg : public figcone::Config {
    FIGCONE_PARAM(test_str, std::string);
};

struct SnakeStructKebabCfg : public figcone::Config {
    FIGCONE_PARAM(test_int, int);
    FIGCONE_NODE(test_inner, InnerSnakeStructKebabCfg);
};

TEST(NameFormat, SnakeStructKebabCfg)
{
    ///test-int = 10
    ///#test-inner:
    ///  test-str = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test-int", "10", {1, 1});
    auto& node = tree.asItem().addNode("test-inner", {2, 1});
    node.asItem().addParam("test-str", "Hello", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::KebabCase};
    auto cfg = cfgReader.read<SnakeStructKebabCfg>("", parser);
    EXPECT_EQ(cfg.test_int, 10);
    EXPECT_EQ(cfg.test_inner.test_str, "Hello");
}

struct InnerCamelStructCamelCfg : public figcone::Config {
    FIGCONE_PARAM(testStr, std::string);
};

struct CamelStructCamelCfg : public figcone::Config {
    FIGCONE_PARAM(testInt, int);
    FIGCONE_NODE(testInner, InnerCamelStructCamelCfg);
};

TEST(NameFormat, CamelStructCamelCfg)
{
    ///testInt = 10
    ///#testInner:
    ///  testStr = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("testInt", "10", {1, 1});
    auto& node = tree.asItem().addNode("testInner", {2, 1});
    node.asItem().addParam("testStr", "Hello", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::CamelCase};
    auto cfg = cfgReader.read<CamelStructCamelCfg>("", parser);
    EXPECT_EQ(cfg.testInt, 10);
    EXPECT_EQ(cfg.testInner.testStr, "Hello");
}

struct InnerCamelStructSnakeCfg : public figcone::Config {
    FIGCONE_PARAM(testStr, std::string);
};

struct CamelStructSnakeCfg : public figcone::Config {
    FIGCONE_PARAM(testInt, int);
    FIGCONE_NODE(testInner, InnerCamelStructSnakeCfg);
};

TEST(NameFormat, CamelStructSnakeCfg)
{
    ///test_int = 10
    ///#test_inner:
    ///  test_str = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test_int", "10", {1, 1});
    auto& node = tree.asItem().addNode("test_inner", {2, 1});
    node.asItem().addParam("test_str", "Hello", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::SnakeCase};
    auto cfg = cfgReader.read<CamelStructCamelCfg>("", parser);
    EXPECT_EQ(cfg.testInt, 10);
    EXPECT_EQ(cfg.testInner.testStr, "Hello");
}

struct InnerCamelStructKebabCfg : public figcone::Config {
    FIGCONE_PARAM(testStr, std::string);
};

struct CamelStructKebabCfg : public figcone::Config {
    FIGCONE_PARAM(testInt, int);
    FIGCONE_NODE(testInner, InnerCamelStructKebabCfg);
};

TEST(NameFormat, CamelStructKebabCfg)
{
    ///test-int = 10
    ///#test-inner:
    ///  test-str = Hello
    ///
    auto tree = figcone::makeTreeRoot();
    tree.asItem().addParam("test-int", "10", {1, 1});
    auto& node = tree.asItem().addNode("test-inner", {2, 1});
    node.asItem().addParam("test-str", "Hello", {3, 3});

    auto parser = TreeProvider{std::move(tree)};
    auto cfgReader = figcone::ConfigReader{figcone::NameFormat::KebabCase};
    auto cfg = cfgReader.read<CamelStructKebabCfg>("", parser);
    EXPECT_EQ(cfg.testInt, 10);
    EXPECT_EQ(cfg.testInner.testStr, "Hello");
}

} //namespace test_nameformat