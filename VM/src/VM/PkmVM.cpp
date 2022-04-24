#include "VM/PkmVM.h"

void PkmVM::destroyVM() {}

Frame* PkmVM::create_new_frame(uint16_t locals_num, ConstPool* const_pool)
{
    Frame* frame = new Frame(locals_num, const_pool);
    stack_frame.push(frame);
    return frame;
}

void PkmVM::delete_top_frame()
{
    Frame* frame = stack_frame.top();
    stack_frame.pop();
    delete frame;
}
