#ifndef VM_PKM_PKMMETHOD_H
#define VM_PKM_PKMMETHOD_H

#include "PkmEnums.h"

#include <string>
#include <vector>

struct PkmClass;
using pclass = PkmClass*;

struct PkmMethod
{
    pkm::AccessType access_type;
    pkm::MethodType modifier;
    pkm::DataType ret_type;
    std::string name;
    std::vector<pkm::DataType> met_params;
    uint32_t offset;
    uint16_t locals_num;
    pclass cls;
};

using pmethodID = PkmMethod*;

#endif // VM_PKM_PKMMETHOD_H