#ifndef VM_PKM_PKMFIELD_H
#define VM_PKM_PKMFIELD_H

#include "PkmEnums.h"

#include <cstdint>

struct PkmField
{
    AccessType access_type;
    VariableType var_type;
    uint16_t name;
};

#endif // VM_PKM_PKMFIELD_H