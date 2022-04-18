#include "Compiler/AST/ASTMaker.h"
#include "Compiler/Compiler.h"
#include "Compiler/Translator/Translator.h"

#include <fstream>

int Compiler::load(const std::string& input_name)
{
    std::ifstream file(input_name);
    if (file.is_open())
    {
        ASTMaker ast_maker(&file);
        ast_maker.make(&ast_);
        if (ast_.branches_num() == 0)
        {
            return FILE_NOT_LOAD;
        }
    }
    else
    {
        return FILE_NOT_FOUND;
    }

    return OK;
}

bool Compiler::compile(const std::string& code_ext)
{
    for (size_t i = 0; i < ast_.branches_num(); i++)
    {
        std::fstream file(static_cast<ClassNode*>(ast_[i].value().get())->name + code_ext, std::ios::out | std::ios::binary);
        if (file.is_open())
        {
            Translator trans(&ast_);
            trans.translate(&file);
        }
        else
        {
            return false;
        }
        file.close();
    }

    return true;
}