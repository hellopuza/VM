#include "VM/Frame.h"

#include <cstring>

Frame::Frame(pmethodID pmet) : pmethod(pmet)
{
    local_variables = new int32_t[pmethod->locals_num] {};
}

Frame::Frame(const Frame& other) : pc(other.pc), pmethod(other.pmethod), operand_stack(other.operand_stack)
{
    local_variables = new int32_t[pmethod->locals_num] {};
    memcpy(local_variables, other.local_variables, pmethod->locals_num * sizeof(int32_t));
}

Frame::Frame(Frame&& other) noexcept :
    pc(other.pc), pmethod(other.pmethod), local_variables(other.local_variables), operand_stack(std::move(other.operand_stack))
{
    other.local_variables = nullptr;
}

Frame::~Frame()
{
    delete[] local_variables;
}

Frame& Frame::operator=(const Frame& other)
{
    if (this == &other)
    {
        return *this;
    }

    pc = other.pc;
    pmethod = other.pmethod;
    operand_stack = other.operand_stack;

    delete[] local_variables;

    local_variables = new int32_t[pmethod->locals_num] {};
    memcpy(local_variables, other.local_variables, pmethod->locals_num * sizeof(int32_t));

    return *this;
}

Frame& Frame::operator=(Frame&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    pc = other.pc;
    pmethod = other.pmethod;
    operand_stack = other.operand_stack;

    delete[] local_variables;

    local_variables = other.local_variables;
    other.local_variables = nullptr;

    return *this;
}