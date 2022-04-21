#ifndef VM_PKM_PKMCLASS_H
#define VM_PKM_PKMCLASS_H

#include "ConstantPool.h"
#include "VM/Pkm/PkmField.h"
#include "VM/Pkm/PkmMethod.h"

#include <unordered_map>

using PkmFields = std::unordered_map<std::string, PkmField>;
using PkmMethods = std::unordered_map<std::string, PkmMethod>;

struct PkmClass
{
    ConstantPool const_pool;
    PkmFields fields;
    PkmMethods methods;
    std::string bytecode;
};

#endif // VM_PKM_PKMCLASS_H