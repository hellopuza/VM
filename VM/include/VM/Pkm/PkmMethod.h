#ifndef VM_PKM_PKMMETHOD_H
#define VM_PKM_PKMMETHOD_H

#include "PkmEnums.h"

#include <string>
#include <vector>

struct PkmClass;
using pclass = PkmClass*;

struct PkmMethod
{
    AccessType access_type;
    MethodType modifier;
    VariableType ret_type;
    uint16_t name;
    uint16_t locals_num;
    uint32_t offset;
    std::vector<VariableType> met_params;
    pclass cls;
};

using pmethodID = PkmMethod*;

#endif // VM_PKM_PKMMETHOD_H