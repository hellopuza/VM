#ifndef VM_PKMVM_H
#define VM_PKMVM_H

#include "VM/ClassLinker.h"
#include "Frame.h"

#include <unordered_map>
#include <stack>

struct PkmVM
{
    PkmVM() = default;
    static void destroyVM();
    Frame* create_new_frame(uint16_t locals_num, ConstPool* const_pool);
    void   delete_top_frame();
    
    std::stack<Frame*> stack_frame;
};

#endif // VM_PKMVM_H