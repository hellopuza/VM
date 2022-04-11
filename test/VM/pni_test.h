#include "VM/PNI.h"

#include <gtest/gtest.h>

TEST(PNITest, CreateVm)
{
    PkmVM* pvm = nullptr;
    PNIEnv* env = nullptr;
    PkmVMInitArgs init_args;
    init_args.files_num = 0;
    init_args.files = nullptr;

    PNI_createVM(&pvm, &env, &init_args);

    pvm->destroyVM();
    delete pvm;
    delete env;
}