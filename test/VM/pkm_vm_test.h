#include "VM/PkmVM.h"
#include "VM/PkmVMInitArgs.h"

#include <gtest/gtest.h>

TEST(PkmVMTest, ArgsCtor)
{
    PkmVM pvm(PkmVMInitArgs());
}