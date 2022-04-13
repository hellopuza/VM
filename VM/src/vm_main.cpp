#include "VM/PNI.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#define PRINT_ERROR(message) \
    std::cout << (message) << "\n"; \
    return -1;

void runVM(int argc, char* argv[]);
int parseFilesNum(int argc, char* argv[]);
char** parseFiles(int files_num, char* argv[]);

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
    init_args.files_num = parseFilesNum(argc, argv);
    init_args.flags_num = argc - init_args.files_num - 1;
    init_args.files_path = parseFiles(init_args.files_num, argv);
    init_args.flags = argv + init_args.files_num;

    PNI_createVM(&pvm, &env, &init_args);

    pvm->destroyVM();
    delete pvm;
    delete env;
}

int parseFilesNum(int argc, char* argv[])
{
    int num = 0;
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            break;
        }
        num++;
    }
    return num;
}

char** parseFiles(int files_num, char* argv[])
{
    char** files = new char*[files_num];
    for(int i = 0; i < files_num; i++)
    {
        source[i] = argv[i + 1];
    }
    return files;
}