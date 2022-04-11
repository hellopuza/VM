#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "Compiler/AST/AST.h"

class Compiler
{
public:
    Compiler() = default;
    bool load(const std::string& input_name);
    bool compile(const std::string& code_ext);

private:
    AST ast_;
};

#endif // COMPILER_COMPILER_H