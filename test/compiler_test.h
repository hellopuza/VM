#include "Compiler/Compiler.h"

#include <gtest/gtest.h> // NOLINT

TEST(CompilerTest, DefaultConstructor) // NOLINT
{
    Compiler comp;
}

TEST(CompilerTest, Load) // NOLINT
{
    Compiler comp;
    EXPECT_FALSE(comp.load(""));
}

TEST(CompilerTest, Compile) // NOLINT
{
    Compiler comp;
    EXPECT_FALSE(comp.compile(""));
}