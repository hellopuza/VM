#include "Compiler/Compiler.h"

#include <gtest/gtest.h> // NOLINT

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define CONSTRUCT_FILE(str)      \
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
    EXPECT_TRUE(comp.compile("!", ".txt") == Compiler::FILE_NOT_FOUND);
}

TEST(CompilerTest, LoadEmptyFile) // NOLINT
{
    CONSTRUCT_FILE(
        ""
    )
    Compiler comp;
    EXPECT_TRUE(comp.compile("file", ".txt") == Compiler::FILE_NOT_COMPILED);
}

TEST(CompilerTest, LoadNormFile) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main;"
    )
    Compiler comp;
    EXPECT_TRUE(comp.compile("file", ".txt") == Compiler::OK);
}

#undef CONSTRUCT_FILE