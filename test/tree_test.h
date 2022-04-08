#include "Common/Tree/Tree-impl.h"

#include <gtest/gtest.h>

TEST(TreeTest, DefaultConstructor)
{
    Tree<int> tree;
}

TEST(TreeTest, ValueConstructor)
{
    Tree<int> tree(1);
    EXPECT_TRUE(tree.value == 1);
}

TEST(TreeTest, CopyConstructor)
{
    Tree<int> tree1(0);
    tree1.emplace_back(Tree<int>(1));
    tree1.emplace_back(Tree<int>(2));
    tree1[0].emplace_back(Tree<int>(3));
    tree1[0].emplace_back(Tree<int>(4));

    Tree<int> tree2(tree1);
    EXPECT_TRUE(tree2.value == 0);
    EXPECT_TRUE(tree2[0].value == 1);
    EXPECT_TRUE(tree2[1].value == 2);
    EXPECT_TRUE(tree2[0][0].value == 3);
    EXPECT_TRUE(tree2[0][1].value == 4);
}

TEST(TreeTest, MoveConstructor)
{
    Tree<int> tree1(0);
    tree1.emplace_back(Tree<int>(1));
    tree1.emplace_back(Tree<int>(2));
    tree1[0].emplace_back(Tree<int>(3));
    tree1[0].emplace_back(Tree<int>(4));

    Tree<int> tree2(std::move(tree1));
    EXPECT_TRUE(tree2.value == 0);
    EXPECT_TRUE(tree2[0].value == 1);
    EXPECT_TRUE(tree2[1].value == 2);
    EXPECT_TRUE(tree2[0][0].value == 3);
    EXPECT_TRUE(tree2[0][1].value == 4);
}

TEST(TreeTest, CopyOp)
{
    Tree<int> tree1(0);
    tree1.emplace_back(Tree<int>(1));
    tree1.emplace_back(Tree<int>(2));
    tree1[0].emplace_back(Tree<int>(3));
    tree1[0].emplace_back(Tree<int>(4));

    Tree<int> tree2;
    tree2 = tree1;
    EXPECT_TRUE(tree2.value == 0);
    EXPECT_TRUE(tree2[0].value == 1);
    EXPECT_TRUE(tree2[1].value == 2);
    EXPECT_TRUE(tree2[0][0].value == 3);
    EXPECT_TRUE(tree2[0][1].value == 4);
}

TEST(TreeTest, MoveOp)
{
    Tree<int> tree1(0);
    tree1.emplace_back(Tree<int>(1));
    tree1.emplace_back(Tree<int>(2));
    tree1[0].emplace_back(Tree<int>(3));
    tree1[0].emplace_back(Tree<int>(4));

    Tree<int> tree2;
    tree2 = std::move(tree1);
    EXPECT_TRUE(tree2.value == 0);
    EXPECT_TRUE(tree2[0].value == 1);
    EXPECT_TRUE(tree2[1].value == 2);
    EXPECT_TRUE(tree2[0][0].value == 3);
    EXPECT_TRUE(tree2[0][1].value == 4);
}

TEST(TreeTest, Dump)
{
    Tree<int> tree(0);
    tree.emplace_back(Tree<int>(1));
    tree.emplace_back(Tree<int>(2));
    tree[0].emplace_back(Tree<int>(3));
    tree[0].emplace_back(Tree<int>(4));

    EXPECT_TRUE(tree.dump("tree_dump") == 0);
}