#ifndef FRAME_H
#define FRAME_H

#include <stack>

#include "VM/Pkm/PkmClass.h"
#include "VM/Pkm/PkmMethod.h"

struct Frame
{
    explicit Frame(pmethodID pmet);
    Frame(const Frame& other);
    Frame(Frame&& other) noexcept;
    ~Frame();

    Frame& operator=(const Frame& other);
    Frame& operator=(Frame&& other) noexcept;

    int32_t pc = 0;

    pmethodID pmethod;
    int32_t*  local_variables;

    std::stack<int32_t> operand_stack;
};

#endif // FRAME_H