#include "Common/tree_test.h"

#include "Compiler/astmaker_test.h"
#include "Compiler/compiler_test.h"

#include "VM/pkm_vm_initargs_test.h"
#include "VM/pkm_vm_test.h"
#include "VM/pni_env_test.h"
#include "VM/pni_test.h"

#include <gtest/gtest.h> // NOLINT

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv); // NOLINT
    return RUN_ALL_TESTS(); // NOLINT
}