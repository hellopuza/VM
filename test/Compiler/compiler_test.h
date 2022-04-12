#include "Compiler/Compiler.h"

#include <gtest/gtest.h> // NOLINT

#include <fstream>
#include <iostream>
#include <string>

/*
#define CONSTRUCT_FILE(str)        \
    std::ofstream ofile(FILENAME); \
    ofile << (str);                \
    ofile.close();                 \
    std::ifstream ifile(FILENAME); \
    ASTMaker ast_maker(&ifile);    \
    AST ast;                       \
    ast_maker.make(&ast); //
*/

TEST(CompilerTest, DefaultConstructor) // NOLINT
{
    Compiler comp;
}

/*
TEST(CompilerTest, LoadEmpty) // NOLINT
{
    Compiler comp;
    EXPECT_FALSE(comp.load(""));
}
*/