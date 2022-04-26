#include "VM/PkmVM.h"

void PkmVM::destroyVM() {}

void PkmVM::create_new_frame(uint16_t locals_num, ConstPool* const_pool)
{
    stack_frame.push(Frame(locals_num, const_pool));
}
