#include "VM/PNI.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#define PRINT_ERROR(message) \
    std::cout << (message) << "\n"; \
    return -1;

void runVM(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    runVM(argc, argv);
    return 0;
}

void runVM(int argc, char* argv[])
{
    PkmVM* pvm = nullptr;
    PNIEnv* env = nullptr;
    PkmVMInitArgs init_args;
    parseCmd(argc, argv, init_args);

    PNI_createVM(&pvm, &env, &init_args);

    pvm->destroyVM();
    delete pvm;
    delete env;
}