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

AST* bindNodes(OperationType op, AST* lhs, AST* rhs);
AST* bindNodes(AST* lhs, AST* rhs);

}
}

%locations

%token

    <NodeType>     CLASS    "class"
    <AccessType>   PRIVATE  "private"
    <AccessType>   PUBLIC   "public"
    <MethodType>   INSTANCE "instance"
    <MethodType>   STATIC   "static"
    <MethodType>   NATIVE   "native"
    <VariableType> VOID     "void"
    <VariableType> BOOLEAN  "boolean"
    <VariableType> BYTE     "byte"
    <VariableType> CHAR     "char"
    <VariableType> SHORT    "short"
    <VariableType> INT      "int"
    <VariableType> LONG     "long"
    <VariableType> FLOAT    "float"
    <VariableType> DOUBLE   "double"

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
    <OperationType> ASSIGN "="
    <OperationType> NEW    "new"
    <OperationType> RETURN "return"

    <ControlType> IF    "if"
    <ControlType> ELSE  "else"
    <ControlType> ELIF  "elif"
    <ControlType> FOR   "for"
    <ControlType> WHILE "while"

    <std::string> NUL   "null"
    <std::string> THIS  "this"
    <bool>        FALSE "false"
    <bool>        TRUE  "true"

    <std::string> WORD
    <int32_t>     INTNUMBER
    <float>       FLOATNUMBER
    <std::string> STRING
    <char>        SYMBOL

    OCB "{"
    CCB "}"
    ORB "("
    CRB ")"
    OSB "["
    CSB "]"
    SCOLON ";"
    ERROR
;

%left '+' '-' '*' '/'

%nterm
    <AST*> CLASS_DECLS
    <AST*> CLASS_DECL

    <std::list<AST*>> CLASS_SCOPE
    <std::list<AST*>> CLASS_FMS

    <AST*> CLASS_FM
    <AST*> FIELD
    <AST*> METHOD

    <AccessType>   ACC
    <VariableType> TYPE
    <VariableType> MTYPE
    <MethodType>   MET

    <std::list<AST*>> MPARAMS
    <std::list<AST*>> FPARAMS
    <std::list<AST*>> PARAMS
    <AST*> PARAM
    <AST*> SCOPE

    <std::list<AST*>> ACTIONS
    <AST*> ACTION

    <AST*> EXPR
    <AST*> CONTROL
    <AST*> ASSIGNMENT
    <AST*> OP_OR
    <AST*> OP_AND
    <AST*> OP_EQ
    <AST*> OP_COMP
    <AST*> OP_ADD
    <AST*> OP_MUL
    <AST*> OP_DOT
    <AST*> CHECK_ASS
    <AST*> CHECK_OR
    <AST*> CHECK_AND
    <AST*> CHECK_EQ
    <AST*> CHECK_COMP
    <AST*> CHECK_ADD
    <AST*> CHECK_MUL
    <AST*> CHECK_DOT
    <AST*> OBJ
    <AST*> EXPRINBR
    <AST*> FUNCTION
    <AST*> VAR_DECL
    <AST*> VAR
    <AST*> NUM
;

%start program

%%

program: CLASS_DECLS                             { maker->ast()->value() = std::make_shared<ASNode>(ASNode()); }

CLASS_DECLS: CLASS_DECL CLASS_DECLS              { maker->ast()->emplace_branch(std::move(*$1)); delete $1; }
           | %empty                              { }
;

CLASS_DECL: CLASS WORD CLASS_SCOPE               { $$ = new AST(std::make_shared<ClassNode>(ClassNode($2))); pushBranches($$, &$3); }

CLASS_SCOPE: OCB CLASS_FMS CCB                   { $$ = std::move($2); }
           | SCOLON                              { }
;

CLASS_FMS: CLASS_FM CLASS_FMS                    { $2.push_front($1); $$ = std::move($2); }
         | %empty                                { }
;

CLASS_FM: FIELD                                  { $$ = $1; }
        | METHOD                                 { $$ = $1; }
;

FIELD: ACC TYPE WORD SCOLON                      { $$ = new AST(std::make_shared<FieldNode>(FieldNode($3, $1, $2))); }

METHOD: ACC MET MTYPE WORD MPARAMS SCOPE         { $$ = new AST(std::make_shared<MethodNode>(MethodNode($4, $1, $2, $3))); pushBranches($$, &$5); $$->emplace_branch(std::move(*$6)); delete $6; }

ACC: PUBLIC                                      { $$ = $1; }
   | PRIVATE                                     { $$ = $1; }
;

TYPE: BOOLEAN                                    { $$ = $1; }
    | BYTE                                       { $$ = $1; }
    | CHAR                                       { $$ = $1; }
    | SHORT                                      { $$ = $1; }
    | INT                                        { $$ = $1; }
    | LONG                                       { $$ = $1; }
    | FLOAT                                      { $$ = $1; }
    | DOUBLE                                     { $$ = $1; }
;

MTYPE: TYPE                                      { $$ = $1; }
     | VOID                                      { $$ = $1; }
;

MET: INSTANCE                                    { $$ = $1; }
   | STATIC                                      { $$ = $1; }
   | NATIVE                                      { $$ = $1; }
;

MPARAMS: ORB PARAMS CRB                          { $$ = std::move($2); }
       | ORB CRB                                 { }

PARAMS: PARAM COMMA PARAMS                       { $3.push_front($1); $$ = std::move($3); }
      | PARAM                                    { $$ = std::list<AST*>(); $$.push_back($1); }
;

PARAM: TYPE WORD                                 { $$ = new AST(std::make_shared<MethodParameterNode>(MethodParameterNode($2, $1))); }

SCOPE: OCB ACTIONS CCB                           { $$ = new AST(std::make_shared<ScopeNode>(ScopeNode())); pushBranches($$, &$2); }

ACTIONS: ACTION ACTIONS                          { $2.push_front($1); $$ = std::move($2); }
       | %empty                                  { }
;

ACTION: EXPR SCOLON                              { $$ = $1; }
      | CONTROL                                  { $$ = $1; }
      | RETURN EXPR SCOLON                       { $$ = new AST(std::make_shared<OperationNode>(OperationNode($1))); $$->emplace_branch(std::move(*$2)); delete $2; }
;

EXPR: CHECK_ASS ASSIGNMENT                       { $$ = bindNodes($1, $2); }

ASSIGNMENT: ASSIGN CHECK_ASS ASSIGNMENT          { $$ = bindNodes($1, $2, $3); }
          | %empty                               { $$ = nullptr; }
;

CHECK_ASS: CHECK_OR OP_OR                        { $$ = bindNodes($1, $2); }

OP_OR: OR CHECK_OR OP_OR                         { $$ = bindNodes($1, $2, $3); }
     | %empty                                    { $$ = nullptr; }
;

CHECK_OR: CHECK_AND OP_AND                       { $$ = bindNodes($1, $2); }

OP_AND: AND CHECK_AND OP_AND                     { $$ = bindNodes($1, $2, $3); }
      | %empty                                   { $$ = nullptr; }
;

CHECK_AND: CHECK_EQ OP_EQ                        { $$ = bindNodes($1, $2); }

OP_EQ: EQ CHECK_EQ OP_EQ                         { $$ = bindNodes($1, $2, $3); }
     | NEQ CHECK_EQ OP_EQ                        { $$ = bindNodes($1, $2, $3); }
     | %empty                                    { $$ = nullptr; }
;

CHECK_EQ: CHECK_COMP OP_COMP                     { $$ = bindNodes($1, $2); }

OP_COMP: STL CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | STG CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | LEQ CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | GEQ CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | %empty                                  { $$ = nullptr; }
;

CHECK_COMP: CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2); }

OP_ADD: ADD CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2, $3); }
      | SUB CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2, $3); }
      | %empty                                   { $$ = nullptr; }
;

CHECK_ADD: CHECK_MUL OP_MUL                      { $$ = bindNodes($1, $2); }

OP_MUL: MUL CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); }
      | DIV CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); }
      | %empty                                   { $$ = nullptr; }
;

CHECK_MUL: CHECK_DOT OP_DOT                      { $$ = bindNodes($1, $2); }

OP_DOT: DOT CHECK_DOT OP_DOT                     { $$ = bindNodes($1, $2, $3); }
      | %empty                                   { $$ = nullptr; }
;

CHECK_DOT: OBJ                                   { $$ = $1; }
         | ADD OBJ                               { $$ = bindNodes($1, $2, nullptr); }
         | SUB OBJ                               { $$ = bindNodes($1, $2, nullptr); }
;

OBJ: EXPRINBR                                    { $$ = $1; }
   | FUNCTION                                    { $$ = $1; }
   | VAR_DECL                                    { $$ = $1; }
   | NUM                                         { $$ = $1; }
   | VAR                                         { $$ = $1; }
;

EXPRINBR: ORB EXPR CRB                           { $$ = $2; }

FUNCTION: WORD ORB FPARAMS CRB                   { $$ = new AST(std::make_shared<FunctionNode>(FunctionNode($1))); pushBranches($$, &$3); }

FPARAMS: EXPR COMMA FPARAMS                      { $3.push_front($1); $$ = std::move($3); }
       | EXPR                                    { $$ = std::list<AST*>(); $$.push_back($1); }
       | %empty                                  { }
;

CONTROL: IF EXPRINBR SCOPE                       { $$ = new AST(std::make_shared<ControlNode>(ControlNode($1))); $$->emplace_branch(std::move(*$2)); delete $2; $$->emplace_branch(std::move(*$3)); delete $3; }
       | ELSE SCOPE                              { $$ = new AST(std::make_shared<ControlNode>(ControlNode($1))); $$->emplace_branch(std::move(*$2)); delete $2; }
       | ELIF EXPRINBR SCOPE                     { $$ = new AST(std::make_shared<ControlNode>(ControlNode($1))); $$->emplace_branch(std::move(*$2)); delete $2; $$->emplace_branch(std::move(*$3)); delete $3; }
       | WHILE EXPRINBR SCOPE                    { $$ = new AST(std::make_shared<ControlNode>(ControlNode($1))); $$->emplace_branch(std::move(*$2)); delete $2; $$->emplace_branch(std::move(*$3)); delete $3; }
;

VAR_DECL: TYPE WORD                              { $$ = new AST(std::make_shared<VariableDeclarationNode>(VariableDeclarationNode($2, $1))); }

VAR: WORD                                        { $$ = new AST(std::make_shared<VariableNode>(VariableNode($1))); }

NUM: INTNUMBER                                   { $$ = new AST(std::make_shared<NumberNode>(NumberNode($1))); }
   | FLOATNUMBER                                 { $$ = new AST(std::make_shared<NumberNode>(NumberNode($1))); }
   | FALSE                                       { $$ = new AST(std::make_shared<NumberNode>(NumberNode(false))); }
   | TRUE                                        { $$ = new AST(std::make_shared<NumberNode>(NumberNode(true))); }
;

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

AST* bindNodes(OperationType op, AST* lhs, AST* rhs)
{
    AST* node = new AST(std::make_shared<OperationNode>(OperationNode(op)));

    if (rhs)
    {
        node->emplace_branch(std::move(*rhs));
        delete rhs;

        (*node)[0].emplace_branch(std::move((*node)[0][0]));
        (*node)[0][0] = std::move(*lhs);
        delete lhs;
    }
    else
    {
        node->emplace_branch(std::move(*lhs));
        delete lhs;
    }

    return node;
}

AST* bindNodes(AST* lhs, AST* rhs)
{
    if (rhs)
    {
        rhs->emplace_branch(std::move((*rhs)[0]));
        (*rhs)[0] = std::move(*lhs);
        delete lhs;
        return rhs;
    }
    return lhs;
}

void parser::error(const parser::location_type&, const std::string&) {}

} // namespace yy