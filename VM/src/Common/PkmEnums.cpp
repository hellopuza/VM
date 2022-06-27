#include "PkmEnums.h"

namespace pkm {

DataType::DataType(VariableType var_type) : type(var_type), name(strings::TYPES[static_cast<int>(var_type)]) {}

DataType::DataType(VariableType var_type, std::string name_) : type(var_type), name(std::move(name_)) {}

} // namespace pkm