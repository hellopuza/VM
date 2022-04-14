#ifndef VM_PKMVMINITARGS_H
#define VM_PKMVMINITARGS_H

struct PkmVMInitArgs
{
    int files_num;
    int flags_num;
    char* files_path;
    char** flags;
};

int parseFilesNum(int argc, char* argv[]);
char** parseFiles(int files_num, char* argv[]);
void parseCmd(PkmVMInitArgs& init_args);

#endif // VM_PKMVMINITARGS_H