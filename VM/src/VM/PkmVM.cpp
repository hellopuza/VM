#include "VM/PkmVM.h"

#include <utility>

PkmVM::PkmVM(PkmVMInitArgs args) : args_(args) {}

void PkmVM::destroyVM()
{
    args_.files = nullptr;
}