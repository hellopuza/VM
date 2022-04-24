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

void pushBranch(AST* root, AST* br);
void pushBranches(AST* root, std::list<AST*>* list);

AST* bindNodes(OperationType op, AST* lhs, AST* rhs = nullptr);
AST* bindNodes(AST* lhs, AST* rhs);

void deleteList(std::list<AST*>* list);

}
}

%locations

%token

    <NodeType>     CLASS
    <AccessType>   PRIVATE
    <AccessType>   PUBLIC
    <MethodType>   INSTANCE
    <MethodType>   STATIC
    <MethodType>   NATIVE
    <VariableType> VOID
    <VariableType> BOOLEAN
    <VariableType> BYTE
    <VariableType> CHAR
    <VariableType> SHORT
    <VariableType> INT
    <VariableType> LONG
    <VariableType> FLOAT
    <VariableType> DOUBLE

    <OperationType> OR
    <OperationType> AND
    <OperationType> EQ
    <OperationType> NEQ
    <OperationType> LEQ
    <OperationType> GEQ
    <OperationType> STL
    <OperationType> STG
    <OperationType> ADD
    <OperationType> SUB
    <OperationType> MUL
    <OperationType> DIV
    <OperationType> COMMA
    <OperationType> ASSIGN
    <OperationType> NEW
    <OperationType> RETURN

    <ControlType> IF
    <ControlType> ELSE
    <ControlType> ELIF
    <ControlType> FOR
    <ControlType> WHILE

    <std::string> NUL
    <std::string> THIS
    <bool>        FALSE
    <bool>        TRUE

    <std::string> WORD
    <std::string> WORD_DOT
    <int32_t>     INTNUMBER
    <float>       FLOATNUMBER
    <std::string> STRING
    <char>        SYMBOL

    OCB
    CCB
    ORB
    CRB
    OSB
    CSB
    SCOLON
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
    <AST*> CHECK_ASS
    <AST*> CHECK_OR
    <AST*> CHECK_AND
    <AST*> CHECK_EQ
    <AST*> CHECK_COMP
    <AST*> CHECK_ADD
    <AST*> CHECK_MUL
    <AST*> OBJ
    <AST*> EXPRINBR
    <AST*> FUNCTION
    <AST*> VAR_DECL
    <AST*> VAR
    <AST*> NUMBER
    <AST*> NEW_OBJ

    <AST*> VAR_SQBR
    <AST*> SQBR
    <std::list<AST*>> SQBRS

    <std::string> WORD_DOT_WORD
;

%start program

%%

program: CLASS_DECLS                             { maker->ast()->value() = std::make_shared<ASNode>(ASNode()); }

CLASS_DECLS: CLASS_DECL CLASS_DECLS              { pushBranch(maker->ast(), $1); }
           | %empty                              { }
;

CLASS_DECL: CLASS WORD CLASS_SCOPE               { $$ = new AST(std::make_shared<ClassNode>(ClassNode($2))); pushBranches($$, &$3); }
          | CLASS WORD error                     { maker->pushTextError("expected ; ", @3); YYABORT; }
          | CLASS error                          { maker->pushTextError("expected class name ", @2); YYABORT; }
;

CLASS_SCOPE: OCB CLASS_FMS CCB                   { $$ = std::move($2); }
           | SCOLON                              { }
           | OCB CLASS_FMS error                 { deleteList(&$2); maker->pushTextError("expected } ", @3); YYABORT; }
;

CLASS_FMS: CLASS_FM CLASS_FMS                    { $2.push_front($1); $$ = std::move($2); }
         | %empty                                { }
;

CLASS_FM: FIELD                                  { $$ = $1; }
        | METHOD                                 { $$ = $1; }
;

FIELD: ACC TYPE WORD SCOLON                      { $$ = new AST(std::make_shared<FieldNode>(FieldNode($3, $1, $2))); }
     | ACC TYPE WORD error                       { maker->pushTextError("expected ; ", @4); YYABORT; }
     | ACC TYPE error                            { maker->pushError("expected field name ", @3); YYABORT; }
     | ACC error                                 { maker->pushError("expected field type ", @2); YYABORT; }
;

METHOD: ACC MET MTYPE WORD MPARAMS SCOPE         { $$ = new AST(std::make_shared<MethodNode>(MethodNode($4, $1, $2, $3))); pushBranches($$, &$5); pushBranch($$, $6); }
      | ACC MET MTYPE WORD MPARAMS error         { maker->pushError("expected { ", @6); YYABORT; }
      | ACC MET MTYPE WORD error                 { maker->pushError("expected ( ", @5); YYABORT; }
      | ACC MET MTYPE error                      { maker->pushError("expected method name ", @4); YYABORT; }
      | ACC MET error                            { maker->pushError("expected method return type ", @3); YYABORT; }
;

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
       | ORB error                               { maker->pushTextError("expected ) ", @2); YYABORT; }
;

PARAMS: PARAM COMMA PARAMS                       { $3.push_front($1); $$ = std::move($3); }
      | PARAM                                    { $$ = std::list<AST*>(); $$.push_back($1); }
      | PARAM error                              { delete $1; maker->pushTextError("expected ) ", @2); YYABORT; }
      | %empty                                   { }
;

PARAM: TYPE WORD                                 { $$ = new AST(std::make_shared<MethodParameterNode>(MethodParameterNode($2, $1))); }
     | TYPE error                                { maker->pushError("expected parameter name ", @2); YYABORT; }
;

SCOPE: OCB ACTIONS CCB                           { $$ = new AST(std::make_shared<ScopeNode>(ScopeNode())); pushBranches($$, &$2); }
     | OCB error                                 { maker->pushTextError("expected } ", @2); YYABORT; }
;

ACTIONS: ACTION ACTIONS                          { $2.push_front($1); $$ = std::move($2); }
       | %empty                                  { }
;

ACTION: EXPR SCOLON                              { $$ = $1; }
      | CONTROL                                  { $$ = $1; }
      | RETURN EXPR SCOLON                       { $$ = new AST(std::make_shared<OperationNode>(OperationNode($1))); pushBranch($$, $2); }
      | RETURN EXPR error                        { delete $2; maker->pushTextError("expected ; ", @3); YYABORT; }
      | EXPR error                               { delete $1; maker->pushTextError("expected ; ", @2); YYABORT; }
;

EXPR: CHECK_ASS ASSIGNMENT                       { $$ = bindNodes($1, $2); }

ASSIGNMENT: ASSIGN CHECK_ASS ASSIGNMENT          { $$ = bindNodes($1, $2, $3); }
          | ASSIGN error                         { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
          | %empty                               { $$ = nullptr; }
;

CHECK_ASS: CHECK_OR OP_OR                        { $$ = bindNodes($1, $2); }

OP_OR: OR CHECK_OR OP_OR                         { $$ = bindNodes($1, $2, $3); }
     | OR error                                  { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
     | %empty                                    { $$ = nullptr; }
;

CHECK_OR: CHECK_AND OP_AND                       { $$ = bindNodes($1, $2); }

OP_AND: AND CHECK_AND OP_AND                     { $$ = bindNodes($1, $2, $3); }
      | AND error                                { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
      | %empty                                   { $$ = nullptr; }
;

CHECK_AND: CHECK_EQ OP_EQ                        { $$ = bindNodes($1, $2); }

OP_EQ: EQ CHECK_EQ OP_EQ                         { $$ = bindNodes($1, $2, $3); }
     | NEQ CHECK_EQ OP_EQ                        { $$ = bindNodes($1, $2, $3); }
     | EQ error                                  { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
     | NEQ error                                 { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
     | %empty                                    { $$ = nullptr; }
;

CHECK_EQ: CHECK_COMP OP_COMP                     { $$ = bindNodes($1, $2); }

OP_COMP: STL CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | STG CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | LEQ CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | GEQ CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | STL error                               { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
       | STG error                               { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
       | LEQ error                               { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
       | GEQ error                               { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
       | %empty                                  { $$ = nullptr; }
;

CHECK_COMP: CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2); }

OP_ADD: ADD CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2, $3); }
      | SUB CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2, $3); }
      | ADD error                                { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
      | SUB error                                { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
      | %empty                                   { $$ = nullptr; }
;

CHECK_ADD: CHECK_MUL OP_MUL                      { $$ = bindNodes($1, $2); }

OP_MUL: MUL CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); }
      | DIV CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); }
      | MUL error                                { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
      | DIV error                                { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
      | %empty                                   { $$ = nullptr; }
;

CHECK_MUL: OBJ                                   { $$ = $1; }
         | ADD OBJ                               { $$ = bindNodes($1, $2, nullptr); }
         | SUB OBJ                               { $$ = bindNodes($1, $2, nullptr); }
         | SUB error                             { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
         | ADD error                             { maker->pushTextError("expected primary-expression ", @2); YYABORT; }
         | ASSIGN error                          { maker->pushError("expected primary-expression before token '='" , @2); YYABORT; }
         | OR error                              { maker->pushError("expected primary-expression before token '||'", @2); YYABORT; }
         | EQ error                              { maker->pushError("expected primary-expression before token '=='", @2); YYABORT; }
         | NEQ error                             { maker->pushError("expected primary-expression before token '!='", @2); YYABORT; }
         | LEQ error                             { maker->pushError("expected primary-expression before token '<='", @2); YYABORT; }
         | GEQ error                             { maker->pushError("expected primary-expression before token '>='", @2); YYABORT; }
         | STL error                             { maker->pushError("expected primary-expression before token '<'" , @2); YYABORT; }
         | STG error                             { maker->pushError("expected primary-expression before token '>'" , @2); YYABORT; }
         | MUL error                             { maker->pushError("expected primary-expression before token '*'" , @2); YYABORT; }
         | DIV error                             { maker->pushError("expected primary-expression before token '/'" , @2); YYABORT; }
         | AND error                             { maker->pushError("expected primary-expression before token '&&'", @2); YYABORT; }
;

OBJ: EXPRINBR                                    { $$ = $1; }
   | FUNCTION                                    { $$ = $1; }
   | VAR_DECL                                    { $$ = $1; }
   | VAR                                         { $$ = $1; }
   | VAR_SQBR                                    { $$ = $1; }
   | NUMBER                                      { $$ = $1; }
   | NEW_OBJ                                     { $$ = $1; }
   | STRING                                      { $1.erase($1.begin()); $1.erase(--$1.end()); $$ = new AST(std::make_shared<StringNode>(StringNode($1))); }
   | SYMBOL                                      { $$ = new AST(std::make_shared<SymbolNode>(SymbolNode($1))); }
;

EXPRINBR: ORB EXPR CRB                           { $$ = $2; }
        | ORB error                              { maker->pushTextError("expected ) ", @2); YYABORT; }
;

FUNCTION: WORD_DOT_WORD ORB FPARAMS CRB          { $$ = new AST(std::make_shared<FunctionNode>(FunctionNode($1))); pushBranches($$, &$3); }
        | WORD_DOT_WORD ORB error                { maker->pushTextError("expected ) ", @3); YYABORT; }
;

FPARAMS: EXPR COMMA FPARAMS                      { $3.push_front($1); $$ = std::move($3); }
       | EXPR                                    { $$ = std::list<AST*>(); $$.push_back($1); }
       | EXPR error                              { delete $1; maker->pushTextError("expected ) ", @2); YYABORT; }
       | %empty                                  { }
;

CONTROL: IF EXPRINBR SCOPE                       { $$ = new AST(std::make_shared<ControlNode>(ControlNode($1))); pushBranch($$, $2); pushBranch($$, $3); }
       | ELSE SCOPE                              { $$ = new AST(std::make_shared<ControlNode>(ControlNode($1))); pushBranch($$, $2); }
       | ELIF EXPRINBR SCOPE                     { $$ = new AST(std::make_shared<ControlNode>(ControlNode($1))); pushBranch($$, $2); pushBranch($$, $3); }
       | WHILE EXPRINBR SCOPE                    { $$ = new AST(std::make_shared<ControlNode>(ControlNode($1))); pushBranch($$, $2); pushBranch($$, $3); }
       | IF EXPRINBR error                       { delete $2; maker->pushTextError("expected { ", @3); YYABORT; }
       | IF error                                { maker->pushTextError("expected ( ", @2); YYABORT; }
       | ELSE error                              { maker->pushTextError("expected { ", @2); YYABORT; }
       | ELIF EXPRINBR error                     { delete $2; maker->pushTextError("expected { ", @3); YYABORT; }
       | ELIF error                              { maker->pushTextError("expected ( ", @2); YYABORT; }
       | WHILE EXPRINBR error                    { delete $2; maker->pushTextError("expected { ", @3); YYABORT; }
       | WHILE error                             { maker->pushTextError("expected ( ", @2); YYABORT; }
;

VAR_DECL: TYPE WORD                              { $$ = new AST(std::make_shared<VariableDeclarationNode>(VariableDeclarationNode($2, $1))); }
        | TYPE error                             { maker->pushError("expected variable name ", @2); YYABORT; }
;

VAR: WORD_DOT_WORD                               { $$ = new AST(std::make_shared<VariableNode>(VariableNode($1))); }

VAR_SQBR: VAR SQBRS                              { $$ = new AST(std::make_shared<OperationNode>(OperationNode(OperationType::SQR_BR))); pushBranch($$, $1); pushBranches($$, &$2); }

SQBRS: SQBR SQBRS                                { $2.push_front($1); $$ = std::move($2); }
     | SQBR                                      { $$ = std::list<AST*>(); $$.push_back($1); }
;

SQBR: OSB EXPR CSB                               { $$ = $2; }
    | OSB EXPR error                             { delete $2; maker->pushTextError("expected ] ", @3); YYABORT; }
    | OSB error                                  { maker->pushTextError("expected ] ", @2); YYABORT; }
;

NUMBER: INTNUMBER                                { $$ = new AST(std::make_shared<NumberNode>(NumberNode($1))); }
      | FLOATNUMBER                              { $$ = new AST(std::make_shared<NumberNode>(NumberNode($1))); }
      | FALSE                                    { $$ = new AST(std::make_shared<NumberNode>(NumberNode(false))); }
      | TRUE                                     { $$ = new AST(std::make_shared<NumberNode>(NumberNode(true))); }
;

NEW_OBJ: NEW WORD                                { $$ = new AST(std::make_shared<OperationNode>(OperationNode($1))); $$->emplace_branch(std::make_shared<TypeNode>(TypeNode($2))); }

WORD_DOT_WORD: WORD                              { $$ = std::move($1); }
             | WORD_DOT                          { $$ = std::move($1); }
;

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, yy::parser::location_type* location, ASTMaker* maker)
{
    return maker->yylex(yylval, location);
}

void pushBranch(AST* root, AST* br)
{
    root->emplace_branch(std::move(*br));
    delete br;
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

void deleteList(std::list<AST*>* list)
{
    for (auto& br : *list)
    {
        delete br;
    }
}

void parser::error(const parser::location_type&, const std::string&) {}

} // namespace yy