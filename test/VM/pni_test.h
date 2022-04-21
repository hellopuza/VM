#include "VM/PNI.h"

#include <gtest/gtest.h> // NOLINT

TEST(PNITest, CreateVm) // NOLINT
{
    PkmVM* pvm = nullptr;
    PNIEnv* env = nullptr;

    PNI_createVM(&pvm, &env);

    pvm->destroyVM();
    delete pvm;
    delete env;
}