#ifndef VM_PKM_PKMFIELD_H
#define VM_PKM_PKMFIELD_H

#include "PkmEnums.h"

struct PkmField
{
    pkm::AccessType access_type;
    pkm::DataType var_type;
    std::string name;
};

#endif // VM_PKM_PKMFIELD_H