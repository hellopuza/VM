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

    void make(AST* ast);
    yy::parser::token_type yylex(yy::parser::semantic_type *yylval, yy::parser::location_type* location) const;
    void pushError(const std::string& error, const yy::location& location);
    void pushTextError(const std::string& error, const yy::location& location);
    std::string* getError();
    void printErrors(std::ostream& os) const;
    bool err() const;
    int lineno() const;
    AST* ast();

private:
    std::vector<std::string> program_;
    std::unique_ptr<Lexer> lexer_;
    std::string errstr_;
    AST* ast_ = nullptr;
};

#endif // COMPILER_AST_ASTMAKER_H