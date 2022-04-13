#include "VM/PkmVMInitArgs.h"

int parseFilesNum(int argc, char* argv[]);
char** parseFiles(int files_num, char* argv[]);
void parseCmd(PkmVMInitArgs& init_args);

void parseCmd(int argc, char* argv[], PkmVMInitArgs& init_args)
{
    init_args->files_num = parseFilesNum(argc, argv);
    init_args->flags_num = argc - init_args->files_num - 1;
    init_args->files_path = parseFiles(init_args->files_num, argv);
    init_args->flags = argv + init_args->files_num;
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
        files[i] = argv[i + 1];
    }
    return files;
}