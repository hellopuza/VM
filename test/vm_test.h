#include "VM/VM.h"

#include <gtest/gtest.h> // NOLINT

TEST(VMTest, RunVm)
{
    char* argv[] = {};
    runVM(1, argv);
}