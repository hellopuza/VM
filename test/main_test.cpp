#include "compiler_test.h"
#include "pkm_vm_initargs_test.h"
#include "pkm_vm_test.h"
#include "pni_env_test.h"
#include "pni_test.h"
#include "tree_test.h"
#include "vm_test.h"

#include <gtest/gtest.h> // NOLINT

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv); // NOLINT
    return RUN_ALL_TESTS(); // NOLINT
}
// Test CI
