#ifndef VM_PKM_PKMMETHOD_H
#define VM_PKM_PKMMETHOD_H

#include "PkmEnums.h"

#include <string>
#include <vector>

struct PkmMethod
{
    AccessType access_type;
    MethodType modifier;
    VariableType ret_type;
    uint16_t name;
    uint32_t offset;
    std::vector<VariableType> met_params;
};

using pmethodID = PkmMethod*;

#endif // VM_PKM_PKMMETHOD_H