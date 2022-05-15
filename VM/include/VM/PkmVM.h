#ifndef VM_PKMVM_H
#define VM_PKMVM_H

#include "Frame.h"

#include <stack>
#include <unordered_map>

struct PkmVM
{
    PkmVM() = default;
    static void destroyVM();
    void create_new_frame(pmethodID pmethod);

    std::stack<Frame> stack_frame;
};

#endif // VM_PKMVM_H