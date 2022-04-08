#include "VM/PNI.h"
#include "VM/VM.h"

void runVM(int argc, char* argv[])
{
    PkmVM* pvm = nullptr;
    PNIEnv* env = nullptr;
    PkmVMInitArgs init_args;
    init_args.files_num = argc;
    init_args.files = argv;

    PNI_createVM(&pvm, &env, &init_args);

    pvm->destroyVM();
    delete pvm;
    delete env;
}