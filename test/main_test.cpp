#include "Common/tree_test.h"

#include "Compiler/astmaker_test.h"
#include "Compiler/compiler_test.h"

#include "VM/pkm_vm_initargs_test.h"
#include "VM/pkm_vm_test.h"
#include "VM/pni_env_test.h"
#include "VM/pni_test.h"
#include "VM/vm_test.h"

#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}