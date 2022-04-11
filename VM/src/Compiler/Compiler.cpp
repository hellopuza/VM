#include "Compiler/AST/ASTMaker.h"
#include "Compiler/Compiler.h"

#include <fstream>

bool Compiler::load(const std::string& input_name)
{
    std::ifstream file(input_name);
    if (file.is_open())
    {
        ASTMaker ast_maker(&file);
        ast_maker.make(&ast_);
    }
    else
    {
        return false;
    }

    return true;
}

bool Compiler::compile(const std::string& code_ext)
{
    for (size_t i = 0; i < ast_.branches_num(); i++)
    {
        std::ofstream file(static_cast<ClassNode*>(ast_[i].value().get())->name + code_ext, std::ofstream::out);
        if (file.is_open())
        {
            file << static_cast<ClassNode*>(ast_[i].value().get())->name;
        }
        else
        {
            return false;
        }
    }

    return true;
}