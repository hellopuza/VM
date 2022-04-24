#ifndef FRAME_H_INCLUDED
#define FRAME_H_INCLUDED

#include <stack>

#include "VM/Pkm/PkmMethod.h"
#include "VM/Pkm/PkmClass.h"

struct Frame
{
    Frame(uint16_t nmb_local_var, ConstPool* const_pool) : const_pool(const_pool)
    {
        local_variables = new int32_t[nmb_local_var] {};
    }
    ~Frame()
    {
        delete[] local_variables;
    }

    ConstPool* const_pool      = nullptr;
    int32_t*   local_variables = nullptr;

    std::stack<int32_t> operand_stack;
};

#endif // FRAME_H_INCLUDED
