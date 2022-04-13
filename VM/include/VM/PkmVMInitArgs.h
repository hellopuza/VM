#ifndef VM_PKMVMINITARGS_H
#define VM_PKMVMINITARGS_H

struct PkmVMInitArgs
{
    int files_num;
    int flags_num;
    char* files_path;
    char** flags;
};

#endif // VM_PKMVMINITARGS_H