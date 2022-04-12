#include "VM/PNI.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#define PRINT_ERROR(message) \
    std::cout << (message) << "\n"; \
    return -1;

const char* const BIN_FOLDER = "./pkm/bin";

void runVM(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    for (const auto& entry : std::filesystem::directory_iterator(BIN_FOLDER))
    {
        std::cout << entry.path() << "\n";
    }
    for (int i = 1; i < argc; i++)
    {
        std::cout << argv[i] << "\n";
    }

    return 0;
}

void runVM(int argc, char* argv[])
{
    PkmVM*  pvm = nullptr;
    PNIEnv* env = nullptr;

    PkmVMInitArgs init_args;
    init_args.files_num = argc;
    init_args.files     = argv;

    PNI_createVM(&pvm, &env, &init_args);

    pvm->destroyVM();
    delete pvm;
    delete env;
}