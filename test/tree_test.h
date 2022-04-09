#include "Common/Tree/Tree-impl.h"

#include <gtest/gtest.h>

TEST(TreeTest, DefaultConstructor)
{
    Tree<int> tree;
    EXPECT_TRUE(tree.size() == 1);
    EXPECT_TRUE(tree.branches_num() == 0);
}

TEST(TreeTest, ValueConstructor)
{
    Tree<int> tree(1);
    EXPECT_TRUE(tree.value() == 1);
    EXPECT_TRUE(tree.size() == 1);
    EXPECT_TRUE(tree.branches_num() == 0);
}

TEST(TreeTest, CopyConstructor)
{
    Tree<int> tree1(0);
    tree1.push_branch(Tree<int>(1));
    tree1.push_branch(Tree<int>(2));
    tree1[0].push_branch(Tree<int>(3));
    tree1[0].push_branch(Tree<int>(4));

    Tree<int> tree2(tree1);
    EXPECT_TRUE(tree2.value() == 0);
    EXPECT_TRUE(tree2[0].value() == 1);
    EXPECT_TRUE(tree2[1].value() == 2);
    EXPECT_TRUE(tree2[0][0].value() == 3);
    EXPECT_TRUE(tree2[0][1].value() == 4);
    EXPECT_TRUE(tree1.size() == tree2.size());
    EXPECT_TRUE(tree1.branches_num() == tree2.branches_num());
}

TEST(TreeTest, MoveConstructor)
{
    Tree<int> tree1(0);
    tree1.push_branch(Tree<int>(1));
    tree1.push_branch(Tree<int>(2));
    tree1[0].push_branch(Tree<int>(3));
    tree1[0].push_branch(Tree<int>(4));

    size_t size = tree1.size();
    size_t branches_num = tree1.branches_num();

    Tree<int> tree2(std::move(tree1));
    EXPECT_TRUE(tree2.value() == 0);
    EXPECT_TRUE(tree2[0].value() == 1);
    EXPECT_TRUE(tree2[1].value() == 2);
    EXPECT_TRUE(tree2[0][0].value() == 3);
    EXPECT_TRUE(tree2[0][1].value() == 4);
    EXPECT_TRUE(tree2.size() == size);
    EXPECT_TRUE(tree2.branches_num() == branches_num);
}

TEST(TreeTest, CopyOp)
{
    Tree<int> tree1(0);
    tree1.push_branch(Tree<int>(1));
    tree1.push_branch(Tree<int>(2));
    tree1[0].push_branch(Tree<int>(3));
    tree1[0].push_branch(Tree<int>(4));

    Tree<int> tree2;
    tree2 = tree1;
    EXPECT_TRUE(tree2.value() == 0);
    EXPECT_TRUE(tree2[0].value() == 1);
    EXPECT_TRUE(tree2[1].value() == 2);
    EXPECT_TRUE(tree2[0][0].value() == 3);
    EXPECT_TRUE(tree2[0][1].value() == 4);
    EXPECT_TRUE(tree1.size() == tree2.size());
    EXPECT_TRUE(tree1.branches_num() == tree2.branches_num());
}

TEST(TreeTest, MoveOp)
{
    Tree<int> tree1(0);
    tree1.push_branch(Tree<int>(1));
    tree1.push_branch(Tree<int>(2));
    tree1[0].push_branch(Tree<int>(3));
    tree1[0].push_branch(Tree<int>(4));

    size_t size = tree1.size();
    size_t branches_num = tree1.branches_num();

    Tree<int> tree2;
    tree2 = std::move(tree1);
    EXPECT_TRUE(tree2.value() == 0);
    EXPECT_TRUE(tree2[0].value() == 1);
    EXPECT_TRUE(tree2[1].value() == 2);
    EXPECT_TRUE(tree2[0][0].value() == 3);
    EXPECT_TRUE(tree2[0][1].value() == 4);
    EXPECT_TRUE(tree2.size() == size);
    EXPECT_TRUE(tree2.branches_num() == branches_num);
}

TEST(TreeTest, AccessOperator)
{
    Tree<int> tree(0);
    tree.push_branch(Tree<int>(1));
    tree.push_branch(Tree<int>(2));
    tree[0].push_branch(Tree<int>(3));
    tree[0].push_branch(Tree<int>(4));

    EXPECT_TRUE(tree.value() == 0);
    EXPECT_TRUE(tree[0].value() == 1);
    EXPECT_TRUE(tree[1].value() == 2);
    EXPECT_TRUE(tree[0][0].value() == 3);
    EXPECT_TRUE(tree[0][1].value() == 4);
}

TEST(TreeTest, Size)
{
    Tree<int> tree(0);
    EXPECT_TRUE(tree.size() == 1);
    tree.push_branch(Tree<int>(1));
    EXPECT_TRUE(tree.size() == 2);
    tree[0].push_branch(Tree<int>(2));
    EXPECT_TRUE(tree.size() == 3);
    tree[0][0].push_branch(Tree<int>(3));
    EXPECT_TRUE(tree.size() == 4);
    tree.pop_branch();
    EXPECT_TRUE(tree.size() == 1);
}

TEST(TreeTest, BranchesNum)
{
    Tree<int> tree(0);
    for (size_t i = 0; i < 100; i++)
    {
        EXPECT_TRUE(tree.branches_num() == i);
        tree.push_branch(Tree<int>(0));
        EXPECT_TRUE(tree.branches_num() == i + 1);
        tree.pop_branch();
        EXPECT_TRUE(tree.branches_num() == i);
        tree.push_branch(Tree<int>(0));
    }
}

TEST(TreeTest, ClearBranches)
{
    Tree<int> tree(0);
    for (size_t i = 0; i < 100; i++)
    {
        tree.push_branch(Tree<int>(0));
    }
    tree.clear_branches();
    EXPECT_TRUE(tree.branches_num() == 0);
    EXPECT_TRUE(tree.size() == 1);
}

TEST(TreeTest, PushBranch)
{
    Tree<int> tree(0);
    for (size_t i = 0; i < 100; i++)
    {
        EXPECT_TRUE(tree.branches_num() == i);
        tree.push_branch(Tree<int>(0));
    }
}

TEST(TreeTest, PopBranch)
{
    Tree<int> tree(0);
    for (size_t i = 0; i < 100; i++)
    {
        tree.push_branch(Tree<int>(0));
        EXPECT_TRUE(tree.branches_num() == i + 1);
        tree.pop_branch();
        EXPECT_TRUE(tree.branches_num() == i);
        tree.push_branch(Tree<int>(0));
    }
}

TEST(TreeTest, DotDump)
{
    Tree<int> tree(0);
    tree.push_branch(Tree<int>(1));
    tree.push_branch(Tree<int>(2));
    tree[0].push_branch(Tree<int>(3));
    tree[0].push_branch(Tree<int>(4));

    EXPECT_TRUE(tree.dot_dump("tree_dump") == 0);
}