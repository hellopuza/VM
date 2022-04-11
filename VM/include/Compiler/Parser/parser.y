%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%param {ASTMaker* maker}

%code requires
{
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include "Compiler/AST/AST.h"

class ASTMaker;
}

%code
{
#include "Compiler/AST/ASTMaker.h"

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, yy::parser::location_type* location, ASTMaker* maker);

}
}

%locations

%token

       <NodeType>     CLASS   "class"
       <AccessType>   PRIVATE "private"
       <AccessType>   PUBLIC  "public"
       <MethodType>   STATIC  "static"
       <MethodType>   NATIVE  "native"
       <VariableType> VOID    "void"
       <VariableType> BOOLEAN "boolean"
       <VariableType> BYTE    "byte"
       <VariableType> CHAR    "char"
       <VariableType> SHORT   "short"
       <VariableType> INT     "int"
       <VariableType> LONG    "long"
       <VariableType> FLOAT   "float"
       <VariableType> DOUBLE  "double"

       <OperationType> OR     "||"
       <OperationType> AND    "&&"
       <OperationType> EQ     "=="
       <OperationType> NEQ    "!="
       <OperationType> LEQ    "<="
       <OperationType> GEQ    ">="
       <OperationType> STL    "<"
       <OperationType> STG    ">"
       <OperationType> ADD    "+"
       <OperationType> SUB    "-"
       <OperationType> MUL    "*"
       <OperationType> DIV    "/"
       <OperationType> DOT    "."
       <OperationType> COMMA  ","
       <OperationType> ASSOP  "="
       <OperationType> NEW    "new"
       <OperationType> RETURN "return"

       <ControlType> IF    "if"
       <ControlType> ELSE  "else"
       <ControlType> FOR   "for"
       <ControlType> WHILE "while"

       <std::string> NUL   "null"
       <std::string> THIS  "this"
       <bool>        FALSE "false"
       <bool>        TRUE  "true"

       <std::string> WORD
       <int64_t>     INTNUMBER
       <double>      FLOATNUMBER
       <std::string> STRING
       <char>        SYMBOL

       OCBRACKET "{"
       CCBRACKET "}"
       ORBRACKET "("
       CRBRACKET ")"
       OSBRACKET "["
       CSBRACKET "]"
       SCOLON ";"
       ERROR
;

%left '+' '-' '*' '/'

%nterm
    <AST*> CLASS_DECLS
    <AST*> CLASS_DECL
;

%start program

%%

program: CLASS_DECLS                             {}

CLASS_DECLS: CLASS_DECL CLASS_DECLS              { $$ = $2; maker->ast()->emplace_branch(std::move($1->value())); delete $1; }
           | %empty                              { $$ = nullptr; }
;

CLASS_DECL: CLASS WORD OCBRACKET CCBRACKET       { $$ = new AST(std::make_unique<ASNode>(ClassNode($2))); }

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, yy::parser::location_type* location, ASTMaker* maker)
{
    return maker->yylex(yylval, location);
}

void parser::error(const parser::location_type&, const std::string&) {}

} // namespace yy