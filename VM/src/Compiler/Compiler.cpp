#include "Compiler/AST/ASTMaker.h"
#include "Compiler/Compiler.h"
#include "Compiler/Translator/Translator.h"

#include <fstream>

int Compiler::compile(const std::string& input_name, const std::string& code_ext)
{
    std::ifstream file(input_name);
    if (file.is_open())
    {
        ASTMaker ast_maker(&file);
        ast_maker.make(&ast_);

        if (ast_maker.err() || (ast_.branches_num() == 0) || !translate(code_ext))
        {
            ast_errors_ = std::move(*ast_maker.getErrors());
            return FILE_NOT_COMPILED;
        }
    }
    else
    {
        return FILE_NOT_FOUND;
    }

    return OK;
}

bool Compiler::translate(const std::string& code_ext)
{
    for (size_t i = 0; i < ast_.branches_num(); i++)
    {
        std::ofstream file(static_cast<ClassNode*>(ast_[i].value().get())->name + code_ext);
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

void Compiler::printErrors(std::ostream& os) const
{
    for (const auto& err: ast_errors_)
    {
        os << err << "\n";
    }
}