#include "VM/PkmVM.h"
#include "VM/PkmVMInitArgs.h"

#include <gtest/gtest.h> // NOLINT

TEST(PkmVMTest, ArgsCtor) // NOLINT and no ... :3 :3 :3
{
    PkmVM pvm(PkmVMInitArgs());
}