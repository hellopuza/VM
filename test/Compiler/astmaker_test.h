#include "Compiler/AST/ASTMaker.h"

#include <fstream>
#include <string>

#include <gtest/gtest.h> // NOLINT

TEST(ASTMakerTest, MakeEmpty) // NOLINT
{
    std::ofstream ofile("empty");
    ofile << " ";
    ofile.close();
    std::ifstream ifile("empty");
    ASTMaker ast_maker(&ifile);
    AST ast;
    ast_maker.make(&ast);
    EXPECT_TRUE(ast.branches_num() == 0);
    EXPECT_TRUE(ast.value().get()->type == NodeType::ROOT);
}