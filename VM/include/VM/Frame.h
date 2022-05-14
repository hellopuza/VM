#ifndef FRAME_H_INCLUDED
#define FRAME_H_INCLUDED

#include <stack>

#include "VM/Pkm/PkmMethod.h"
#include "VM/Pkm/PkmClass.h"

struct Frame
{
    explicit Frame(pmethodID pmethod) : pmethod(pmethod)
    {
        local_variables = new int32_t[pmethod->locals_num] {};
    }
    ~Frame()
    {
        delete[] local_variables;
    }

    int32_t pc = 0;

    pmethodID pmethod         = nullptr;
    int32_t*  local_variables = nullptr;

    std::stack<int32_t> operand_stack;
};

#endif // FRAME_H_INCLUDED
