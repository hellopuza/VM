#include "compiler_test.h"
#include "pkm_vm_initargs_test.h"
#include "pkm_vm_test.h"
#include "pni_env_test.h"
#include "pni_test.h"
#include "vm_test.h"

#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}