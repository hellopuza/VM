#include "Compiler/Compiler.h"

#include <gtest/gtest.h>

TEST(CompilerTest, DefaultConstructor)
{
    Compiler comp;
}

TEST(CompilerTest, Load)
{
    Compiler comp;
    EXPECT_FALSE(comp.load(""));
}

TEST(CompilerTest, Compile)
{
    Compiler comp;
    EXPECT_FALSE(comp.compile(""));
}