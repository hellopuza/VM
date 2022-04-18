#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "Compiler/AST/AST.h"

class Compiler
{
public:
    enum Errors
    {
        OK,
        FILE_NOT_LOAD,
        FILE_NOT_FOUND,
    };

    Compiler() = default;
    int load(const std::string& input_name);
    bool compile(const std::string& code_ext);

private:
    AST ast_;
};

#endif // COMPILER_COMPILER_H