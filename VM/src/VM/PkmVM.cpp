#include "VM/PkmVM.h"

void PkmVM::destroyVM() {}

void PkmVM::create_new_frame(pmethodID pmethod)
{
    stack_frame.push(Frame(pmethod));
}
