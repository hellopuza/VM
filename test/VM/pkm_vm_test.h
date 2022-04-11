#include "VM/PkmVM.h"
#include "VM/PkmVMInitArgs.h"

#include <gtest/gtest.h> // NOLINT

TEST(PkmVMTest, ArgsCtor) // NOLINT
{
    PkmVM pvm(PkmVMInitArgs());
}