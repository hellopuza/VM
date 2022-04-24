#ifndef VM_PKMVM_H
#define VM_PKMVM_H

#include "VM/ClassLinker.h"

#include <unordered_map>
#include <stack>

struct PkmVM
{
    PkmVM() = default;
    static void destroyVM();

    std::stack<int32_t> stack;
};

#endif // VM_PNI_H