#ifndef VM_PKMVM_H
#define VM_PKMVM_H

#include "VM/PkmVMInitArgs.h"
#include "VM/Interpreter.h"

class PkmVM
{
public:
    explicit PkmVM(PkmVMInitArgs args);
    void destroyVM();

private:
    PkmVMInitArgs args_;
    Interpreter interpreter_;
};

#endif // VM_PNI_H