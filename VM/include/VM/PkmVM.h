#ifndef VM_PKMVM_H
#define VM_PKMVM_H

#include "PkmVMInitArgs.h"

class PkmVM
{
public:
    explicit PkmVM(PkmVMInitArgs args);
    void destroyVM();

private:
    PkmVMInitArgs args_;
};

#endif // VM_PNI_H