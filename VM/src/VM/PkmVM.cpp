#include "VM/PkmVM.h"

#include <utility>

PkmVM::PkmVM(PkmVMInitArgs args) : args_(args) {}

void PkmVM::destroyVM()
{
    delete [] args_.files_path;
}