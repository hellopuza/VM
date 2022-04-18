#include "Compiler/Compiler.h"

#include <gtest/gtest.h> // NOLINT

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define MAKE_FILE(str)           \
    std::ofstream ofile("file"); \
    ofile << (str);              \
    ofile.close(); //

TEST(CompilerTest, DefaultConstructor) // NOLINT
{
    Compiler comp;
}

TEST(CompilerTest, LoadWrongFileName) // NOLINT
{
    Compiler comp;
    EXPECT_TRUE(comp.load("!") == Compiler::FILE_NOT_FOUND);
}

TEST(CompilerTest, LoadEmptyFile) // NOLINT
{
    MAKE_FILE(
        ""
    )
    Compiler comp;
    EXPECT_TRUE(comp.load("file") == Compiler::FILE_NOT_LOAD);
}

TEST(CompilerTest, LoadNormFile) // NOLINT
{
    MAKE_FILE(
        "class Main;"
    )
    Compiler comp;
    EXPECT_TRUE(comp.load("file") == Compiler::OK);
}