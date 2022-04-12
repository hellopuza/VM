%language "c++"

%skeleton "lalr1.cc"
%defines
%define api.value.type variant
%param {ASTMaker* maker}

%code requires
{
#include "Compiler/AST/AST.h"

#include <algorithm>
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <utility>

class ASTMaker;
}

%code
{
#include "Compiler/AST/ASTMaker.h"

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, yy::parser::location_type* location, ASTMaker* maker);

void pushBranches(AST* root, std::list<AST*>* list);

}
}

%locations

%token

    <NodeType>     CLASS   "class"
    <AccessType>   PRIVATE "private"
    <AccessType>   PUBLIC  "public"
    <MethodType>   NSTATIC
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

    <std::list<AST*>> CLASS_SCOPE
    <std::list<AST*>> FIELDS
    <std::list<AST*>> METHODS

    <AST*> FIELD
    <AST*> METHOD

    <AccessType>   ACC
    <VariableType> VAR
    <VariableType> MVAR
    <MethodType>   MET

    <std::list<AST*>> MPARAMS
    <std::list<AST*>> PARAMS
    <AST*> PARAM
    <AST*> SCOPE
;

%start program

%%

program: CLASS_DECLS                             { maker->ast()->value() = std::make_shared<ASNode>(ASNode()); }

CLASS_DECLS: CLASS_DECL CLASS_DECLS              { maker->ast()->emplace_branch(std::move(*$1)); delete $1; }
           | %empty                              { }
;

CLASS_DECL: CLASS WORD CLASS_SCOPE               { $$ = new AST(std::make_shared<ClassNode>(ClassNode($2))); pushBranches($$, &$3); }

CLASS_SCOPE: OCBRACKET FIELDS METHODS CCBRACKET  { $2.splice($2.end(), $3); $$ = std::move($2); }
           | SCOLON                              { }
;

FIELDS: FIELDS FIELD                             { $1.push_back($2); $$ = std::move($1); }
      | %empty                                   { }
;

METHODS: METHOD METHODS                          { $2.push_front($1); $$ = std::move($2); }
       | %empty                                  { }
;

FIELD: ACC VAR WORD SCOLON                       { $$ = new AST(std::make_shared<FieldNode>(FieldNode($3, $1, $2))); }

METHOD: ACC MET MVAR WORD MPARAMS SCOPE          { $$ = new AST(std::make_shared<MethodNode>(MethodNode($4, $1, $2, $3))); pushBranches($$, &$5); $$->emplace_branch(std::move(*$6)); delete $6; }

ACC: PUBLIC                                      { $$ = $1; }
   | PRIVATE                                     { $$ = $1; }
;

VAR: BOOLEAN                                     { $$ = $1; }
   | BYTE                                        { $$ = $1; }
   | CHAR                                        { $$ = $1; }
   | SHORT                                       { $$ = $1; }
   | INT                                         { $$ = $1; }
   | LONG                                        { $$ = $1; }
   | FLOAT                                       { $$ = $1; }
   | DOUBLE                                      { $$ = $1; }
;

MVAR: VAR                                        { $$ = $1; }
    | VOID                                       { $$ = $1; }
;

MET: STATIC                                      { $$ = $1; }
   | NATIVE                                      { $$ = $1; }
   | %empty                                      { $$ = MethodType::NONSTATIC; }
;

MPARAMS: ORBRACKET PARAMS CRBRACKET              { $$ = std::move($2); }
       | ORBRACKET CRBRACKET                     { }

PARAMS: PARAM COMMA PARAMS                       { $3.push_front($1); $$ = std::move($3); }
      | PARAM                                    { $$ = std::list<AST*>(); $$.push_back($1); }
;

PARAM: VAR WORD                                  { $$ = new AST(std::make_shared<MethodParameterNode>(MethodParameterNode($2, $1))); }

SCOPE: OCBRACKET CCBRACKET                       { $$ = new AST(std::make_shared<ScopeNode>(ScopeNode())); }

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, yy::parser::location_type* location, ASTMaker* maker)
{
    return maker->yylex(yylval, location);
}

void pushBranches(AST* root, std::list<AST*>* list)
{
    for (auto& br : *list)
    {
        root->emplace_branch(std::move(*br));
        delete br;
    }
}

void parser::error(const parser::location_type&, const std::string&) {}

} // namespace yy