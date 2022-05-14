#ifndef FRAME_H_INCLUDED
#define FRAME_H_INCLUDED

#include <stack>
#include <iostream>
#include <cstring>

#include "VM/Pkm/PkmMethod.h"
#include "VM/Pkm/PkmClass.h"

struct Frame
{
    explicit Frame(pmethodID pmet) : pmethod(pmet)
    {
        local_variables = new int32_t[pmethod->locals_num] {};
    }
    Frame(const Frame& other) : pc(other.pc), pmethod(other.pmethod), operand_stack(other.operand_stack)
    {
        local_variables = new int32_t[pmethod->locals_num] {};
        memcpy(local_variables, other.local_variables, pmethod->locals_num * sizeof(int32_t));
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
