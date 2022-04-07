#include <gtest/gtest.h>

TEST(TestTest, Test)
{
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}