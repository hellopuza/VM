#ifndef VM_PKMVM_H
#define VM_PKMVM_H

#include "VM/ClassLinker.h"

#include <unordered_map>

class PkmVM
{
public:
    PkmVM() = default;
    static void destroyVM();
};

#endif // VM_PNI_H