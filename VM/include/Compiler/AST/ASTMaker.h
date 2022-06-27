#ifndef COMPILER_AST_ASTMAKER_H
#define COMPILER_AST_ASTMAKER_H

#include "Compiler/AST/AST.h"
#include "Compiler/Lexer/Lexer.h"
#include "parser.hh"

#include <fstream>
#include <string>
#include <vector>

class ASTMaker
{
public:
    ASTMaker(std::ifstream* file);
    void make(ast::AST* ast);

    yy::parser::token_type yylex(yy::parser::semantic_type *yylval, yy::parser::location_type* location) const;
    void pushError(CompilationError::Type type, const yy::location& location);
    CompilationError getError() const;
    ast::AST* ast();

private:
    std::unique_ptr<Lexer> lexer_;
    CompilationError error_;
    ast::AST* ast_ = nullptr;
};

#endif // COMPILER_AST_ASTMAKER_H