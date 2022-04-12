#ifndef VM_PKMVM_H
#define VM_PKMVM_H

#include "VM/PkmVMInitArgs.h"
#include "VM/Interpreter.h"

#include <string>

class PkmVM
{
public:
    explicit PkmVM(const PkmVMInitArgs args);
    void destroyVM();

private:
    PkmVMInitArgs args_;
    Interpreter interpreter_;
};

#endif // VM_PNI_H