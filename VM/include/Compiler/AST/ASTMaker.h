#ifndef COMPILER_AST_ASTMAKER_H
#define COMPILER_AST_ASTMAKER_H

#include "Compiler/AST/AST.h"
#include "Compiler/Lexer/Lexer.h"
#include "parser.hh"

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include <fstream>
#include <string>
#include <vector>

class ASTMaker
{
public:
    ASTMaker(std::ifstream* file);

    void make(AST* ast);
    yy::parser::token_type yylex(yy::parser::semantic_type *yylval, yy::parser::location_type* location);
    int lineno();
    AST* ast();

private:
    std::vector<std::string> program_;
    std::unique_ptr<Lexer> lexer_;
    AST* ast_ = nullptr;
};

#endif // COMPILER_AST_ASTMAKER_H