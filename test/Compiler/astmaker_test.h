#include "Compiler/AST/ASTMaker.h"

#include <fstream>
#include <string>

#include <gtest/gtest.h>

TEST(ASTMakerTest, MakeEmpty)
{
    std::ifstream file;
    file << "";

    ASTMaker ast_maker(&file);
    AST ast;
    ast_maker.make(&ast);
}