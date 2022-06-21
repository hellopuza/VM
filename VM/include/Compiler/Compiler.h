#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "Compiler/AST/AST.h"

class Compiler
{
public:
    enum Errors
    {
        OK,
        FILE_NOT_COMPILED,
        FILE_NOT_FOUND,
    };

    Compiler() = default;
    int compile(const std::string& input_name, const std::string& code_ext);
    void printErrors(std::ostream& os) const;

private:
    int translate(const std::string& code_ext);

    AST ast_;
    std::string errstr_;
};

#endif // COMPILER_COMPILER_H