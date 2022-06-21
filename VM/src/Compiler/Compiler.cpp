#include "Compiler/AST/ASTMaker.h"
#include "Compiler/Compiler.h"
#include "Compiler/Translator/Translator.h"

#include <fstream>

int Compiler::compile(const std::string& input_name, const std::string& code_ext)
{
    int err = parse(input_name);
    if (err)
    {
        return err;
    }

    err = translate(code_ext);

    return err;
}

int Compiler::parse(const std::string& input_name)
{
    std::ifstream file(input_name);
    if (file.is_open())
    {
        ASTMaker ast_maker(&file);
        ast_maker.make(&ast_);

        if (ast_maker.err() || (ast_.branches_num() == 0))
        {
            errstr_ = std::move(*ast_maker.getError());
            return FILE_NOT_COMPILED;
        }
    }
    else
    {
        return FILE_NOT_FOUND;
    }

    return OK;
}

int Compiler::translate(const std::string& code_ext)
{
    for (size_t i = 0; i < ast_.branches_num(); i++)
    {
        std::ofstream file(static_cast<ClassNode*>(ast_[i].value().get())->name + code_ext);
        if (file.is_open())
        {
            Translator trans(static_cast<AST*>(&ast_[i]));
            trans.translate(&file);
        }
        else
        {
            return FILE_NOT_COMPILED;
        }

        file.close();
    }

    return OK;
}

void Compiler::printErrors(std::ostream& os) const
{
    if (!errstr_.empty())
    {
        os << errstr_ << "\n";
    }
}