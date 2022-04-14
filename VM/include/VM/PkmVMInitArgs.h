#ifndef VM_PKMVMINITARGS_H
#define VM_PKMVMINITARGS_H

struct PkmVMInitArgs
{
    int files_num;
    int flags_num;
    const char** files_path;
    const char** flags;
};

int parseFilesNum(int argc, const char* argv[]);
const char** parseFiles(int files_num, const char* argv[]);
void parseCmd(int argc, const char* argv[], PkmVMInitArgs& init_args);

#endif // VM_PKMVMINITARGS_H