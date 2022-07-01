#ifndef FRAME_TEST_INCLUDED
#define FRAME_TEST_INCLUDED

#include "VM/Frame.h"

#include <gtest/gtest.h> // NOLINT

PkmMethod met = {
    pkm::AccessType::PRIVATE,
    pkm::MethodType::INSTANCE,
    pkm::DataType(pkm::VariableType::INT),
    "a",
    std::vector<pkm::DataType>(),
    2,
    0,
    nullptr
};

PkmMethod met1 = {
    pkm::AccessType::PUBLIC,
    pkm::MethodType::STATIC,
    pkm::DataType(pkm::VariableType::VOID),
    "b",
    std::vector<pkm::DataType>(),
    5,
    1,
    nullptr
};

pmethodID pmet = &met;

TEST(FrameTest, Constructor)
{
    Frame frame(pmet);

    EXPECT_TRUE(frame.pc == 0);
    EXPECT_TRUE(frame.pmethod == pmet);
    EXPECT_TRUE(frame.pmethod->locals_num == met.locals_num);
    EXPECT_FALSE(frame.local_variables == nullptr);
    EXPECT_TRUE(frame.operand_stack.size() == 0);
}

TEST(FrameTest, CopyConstructor) // NOLINT
{
    Frame frame1(pmet);
    Frame frame(frame1);

    EXPECT_TRUE(frame.pc == 0);
    EXPECT_TRUE(frame.pmethod == pmet);
    EXPECT_TRUE(frame.pmethod->locals_num == met.locals_num);
    EXPECT_FALSE(frame.local_variables == nullptr);
    EXPECT_TRUE(frame.operand_stack.size() == 0);
}

TEST(FrameTest, MoveConstructor) // NOLINT
{
    Frame frame1(pmet);
    Frame frame(std::move(frame1));

    EXPECT_TRUE(frame.pc == 0);
    EXPECT_TRUE(frame.pmethod == pmet);
    EXPECT_TRUE(frame.pmethod->locals_num == met.locals_num);
    EXPECT_FALSE(frame.local_variables == nullptr);
    EXPECT_TRUE(frame.operand_stack.size() == 0);
}

TEST(FrameTest, CopyOp) // NOLINT
{
    Frame frame1(pmet);
    Frame frame(&met1);
    frame = frame1;

    EXPECT_TRUE(frame.pc == 0);
    EXPECT_TRUE(frame.pmethod == pmet);
    EXPECT_TRUE(frame.pmethod->locals_num == met.locals_num);
    EXPECT_FALSE(frame.local_variables == nullptr);
    EXPECT_TRUE(frame.operand_stack.size() == 0);
}

TEST(FrameTest, MoveOp) // NOLINT
{
    Frame frame1(pmet);
    Frame frame(&met1);
    frame = std::move(frame1);

    EXPECT_TRUE(frame.pc == 0);
    EXPECT_TRUE(frame.pmethod == pmet);
    EXPECT_TRUE(frame.pmethod->locals_num == met.locals_num);
    EXPECT_FALSE(frame.local_variables == nullptr);
    EXPECT_TRUE(frame.operand_stack.size() == 0);
}

#endif //FRAME_TEST_INCLUDED