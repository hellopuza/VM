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

using astp = std::shared_ptr<AST>;

class ASTMaker;
}

%code
{
#include "Compiler/AST/ASTMaker.h"

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, yy::parser::location_type* location, ASTMaker* maker);

void pushBranch(AST* root, astp br);
void pushBranches(AST* root, std::list<astp>* list);

astp bindNodes(OperationType op, astp lhs, astp rhs = nullptr);
astp bindNodes(astp lhs, astp rhs);

}
}

%locations

%token

    <ASTNodeType>  CLASS
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

    <OperationType> RETURN
    <OperationType> ASSIGN
    <OperationType> OR
    <OperationType> AND
    <OperationType> EQ
    <OperationType> NEQ
    <OperationType> LEQ
    <OperationType> GEQ
    <OperationType> STL
    <OperationType> STG
    <OperationType> SHL
    <OperationType> SHR
    <OperationType> ADD
    <OperationType> SUB
    <OperationType> MUL
    <OperationType> DIV
    <OperationType> REM
    <OperationType> NOT
    <OperationType> NEW

    <ControlType> IF
    <ControlType> ELSE
    <ControlType> ELIF
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
    COMMA
;

%left '+' '-' '*' '/'

%nterm
    <astp> CLASS_DECLS
    <astp> CLASS_DECL

    <std::list<astp>> CLASS_SCOPE
    <std::list<astp>> CLASS_FMS

    <astp> CLASS_FM
    <astp> FIELD
    <astp> METHOD

    <AccessType>   ACC
    <VariableType> TYPE
    <VariableType> MTYPE
    <MethodType>   MET

    <std::list<astp>> MPARAMS
    <std::list<astp>> FPARAMS
    <std::list<astp>> PARAMS
    <astp> PARAM
    <astp> SCOPE

    <std::list<astp>> ACTIONS
    <astp> ACTION
    <astp> EXPR

    <astp> CONTROL
    <astp> IF_ELIF_ELSE
    <astp> ELIF_ELSE
    <astp> IF_SCOPE
    <astp> ELIF_SCOPE
    <astp> ELSE_SCOPE
    <astp> WHILE_SCOPE

    <astp> ASSIGNMENT
    <astp> OP_OR
    <astp> OP_AND
    <astp> OP_EQ
    <astp> OP_COMP
    <astp> OP_SHIFT
    <astp> OP_ADD
    <astp> OP_MUL
    <astp> CHECK_ASS
    <astp> CHECK_OR
    <astp> CHECK_AND
    <astp> CHECK_EQ
    <astp> CHECK_COMP
    <astp> CHECK_SHIFT
    <astp> CHECK_ADD
    <astp> CHECK_MUL
    <astp> NOT_OBJ
    <astp> OBJ
    <astp> EXPRINBR
    <astp> FUNCTION
    <astp> VAR_DECL
    <astp> VAR
    <astp> NUMBER
    <astp> NEW_OBJ

    <std::list<astp>> SQBRS
    <astp> VAR_SQBR
    <astp> SQBR

    <std::string> WORD_DOT_WORD
;

%start program

%%

program: CLASS_DECLS                             { maker->ast()->value() = std::make_shared<ASNode>(ASNode()); }

CLASS_DECLS: CLASS_DECL CLASS_DECLS              { pushBranch(maker->ast(), $1); }
           | %empty                              { }
;

CLASS_DECL: CLASS WORD CLASS_SCOPE               { $$ = std::make_shared<AST>(std::make_shared<ClassNode>(ClassNode($2))); pushBranches($$.get(), &$3); }
          | CLASS WORD error                     { maker->pushTextError("expected ;", @3); YYABORT; }
          | CLASS error                          { maker->pushTextError("expected class name", @2); YYABORT; }
          | error                                { maker->pushError("expected class", @1); YYABORT; }
;

CLASS_SCOPE: OCB CLASS_FMS CCB                   { $$ = std::move($2); }
           | SCOLON                              { }
           | OCB CLASS_FMS error                 { maker->pushTextError("expected }", @3); YYABORT; }
;

CLASS_FMS: CLASS_FM CLASS_FMS                    { $2.push_front($1); $$ = std::move($2); }
         | %empty                                { }
;

CLASS_FM: FIELD                                  { $$ = $1; }
        | METHOD                                 { $$ = $1; }
;

FIELD: ACC TYPE WORD SCOLON                      { $$ = std::make_shared<AST>(std::make_shared<FieldNode>(FieldNode($3, $1, $2))); }
     | ACC TYPE WORD error                       { maker->pushTextError("expected ;", @4); YYABORT; }
     | ACC TYPE error                            { maker->pushError("expected field name", @3); YYABORT; }
     | ACC error                                 { maker->pushError("expected field type", @2); YYABORT; }
;

METHOD: ACC MET MTYPE WORD MPARAMS SCOPE         { $$ = std::make_shared<AST>(std::make_shared<MethodNode>(MethodNode($4, $1, $2, $3))); pushBranches($$.get(), &$5); pushBranch($$.get(), $6); }
      | ACC MET MTYPE WORD MPARAMS error         { maker->pushError("expected {", @6); YYABORT; }
      | ACC MET MTYPE WORD error                 { maker->pushError("expected (", @5); YYABORT; }
      | ACC MET MTYPE error                      { maker->pushError("expected method name", @4); YYABORT; }
      | ACC MET error                            { maker->pushError("expected method return type", @3); YYABORT; }
;

ACC: PUBLIC                                      { $$ = $1; }
   | PRIVATE                                     { $$ = $1; }
;

TYPE: CHAR                                       { $$ = $1; }
    | INT                                        { $$ = $1; }
    | FLOAT                                      { $$ = $1; }
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
       | ORB error                               { maker->pushTextError("expected )", @2); YYABORT; }
;

PARAMS: PARAM COMMA PARAMS                       { $3.push_front($1); $$ = std::move($3); }
      | PARAM                                    { $$ = std::list<astp>(); $$.push_back($1); }
      | PARAM error                              { maker->pushTextError("expected )", @2); YYABORT; }
;

PARAM: TYPE WORD                                 { $$ = std::make_shared<AST>(std::make_shared<MethodParameterNode>(MethodParameterNode($2, $1))); }
     | TYPE error                                { maker->pushError("expected parameter name", @2); YYABORT; }
;

SCOPE: OCB ACTIONS CCB                           { $$ = std::make_shared<AST>(std::make_shared<ScopeNode>(ScopeNode())); pushBranches($$.get(), &$2); }
     | OCB error                                 { maker->pushTextError("expected }", @2); YYABORT; }
;

ACTIONS: ACTION ACTIONS                          { $2.push_front($1); $$ = std::move($2); }
       | %empty                                  { }
;

ACTION: EXPR SCOLON                              { $$ = $1; }
      | CONTROL                                  { $$ = $1; }
      | RETURN EXPR SCOLON                       { $$ = std::make_shared<AST>(std::make_shared<OperationNode>(OperationNode($1))); pushBranch($$.get(), $2); }
      | RETURN SCOLON                            { $$ = std::make_shared<AST>(std::make_shared<OperationNode>(OperationNode($1))); }
      | RETURN EXPR error                        { maker->pushTextError("expected ;", @3); YYABORT; }
      | RETURN error                             { maker->pushTextError("expected ;", @2); YYABORT; }
      | EXPR error                               { maker->pushTextError("expected ;", @2); YYABORT; }
;

EXPR: CHECK_ASS ASSIGNMENT                       { $$ = bindNodes($1, $2); }

ASSIGNMENT: ASSIGN CHECK_ASS ASSIGNMENT          { $$ = bindNodes($1, $2, $3); }
          | ASSIGN error                         { maker->pushTextError("expected primary-expression", @2); YYABORT; }
          | %empty                               { $$ = nullptr; }
;

CHECK_ASS: CHECK_OR OP_OR                        { $$ = bindNodes($1, $2); }

OP_OR: OR CHECK_OR OP_OR                         { $$ = bindNodes($1, $2, $3); }
     | OR error                                  { maker->pushTextError("expected primary-expression", @2); YYABORT; }
     | %empty                                    { $$ = nullptr; }
;

CHECK_OR: CHECK_AND OP_AND                       { $$ = bindNodes($1, $2); }

OP_AND: AND CHECK_AND OP_AND                     { $$ = bindNodes($1, $2, $3); }
      | AND error                                { maker->pushTextError("expected primary-expression", @2); YYABORT; }
      | %empty                                   { $$ = nullptr; }
;

CHECK_AND: CHECK_EQ OP_EQ                        { $$ = bindNodes($1, $2); }

OP_EQ: EQ CHECK_EQ OP_EQ                         { $$ = bindNodes($1, $2, $3); }
     | NEQ CHECK_EQ OP_EQ                        { $$ = bindNodes($1, $2, $3); }
     | EQ error                                  { maker->pushTextError("expected primary-expression", @2); YYABORT; }
     | NEQ error                                 { maker->pushTextError("expected primary-expression", @2); YYABORT; }
     | %empty                                    { $$ = nullptr; }
;

CHECK_EQ: CHECK_COMP OP_COMP                     { $$ = bindNodes($1, $2); }

OP_COMP: STL CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | STG CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | LEQ CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | GEQ CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); }
       | STL error                               { maker->pushTextError("expected primary-expression", @2); YYABORT; }
       | STG error                               { maker->pushTextError("expected primary-expression", @2); YYABORT; }
       | LEQ error                               { maker->pushTextError("expected primary-expression", @2); YYABORT; }
       | GEQ error                               { maker->pushTextError("expected primary-expression", @2); YYABORT; }
       | %empty                                  { $$ = nullptr; }
;

CHECK_COMP: CHECK_SHIFT OP_SHIFT                 { $$ = bindNodes($1, $2); }

OP_SHIFT: SHL CHECK_SHIFT OP_SHIFT               { $$ = bindNodes($1, $2, $3); }
        | SHR CHECK_SHIFT OP_SHIFT               { $$ = bindNodes($1, $2, $3); }
        | SHL error                              { maker->pushTextError("expected primary-expression", @2); YYABORT; }
        | SHR error                              { maker->pushTextError("expected primary-expression", @2); YYABORT; }
        | %empty                                 { $$ = nullptr; }
;

CHECK_SHIFT: CHECK_ADD OP_ADD                    { $$ = bindNodes($1, $2); }

OP_ADD: ADD CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2, $3); }
      | SUB CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2, $3); }
      | ADD error                                { maker->pushTextError("expected primary-expression", @2); YYABORT; }
      | SUB error                                { maker->pushTextError("expected primary-expression", @2); YYABORT; }
      | %empty                                   { $$ = nullptr; }
;

CHECK_ADD: CHECK_MUL OP_MUL                      { $$ = bindNodes($1, $2); }

OP_MUL: MUL CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); }
      | DIV CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); }
      | REM CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); }
      | MUL error                                { maker->pushTextError("expected primary-expression", @2); YYABORT; }
      | DIV error                                { maker->pushTextError("expected primary-expression", @2); YYABORT; }
      | REM error                                { maker->pushTextError("expected primary-expression", @2); YYABORT; }
      | %empty                                   { $$ = nullptr; }
;

CHECK_MUL: NOT_OBJ                               { $$ = $1; }
         | ADD NOT_OBJ                           { $$ = bindNodes($1, $2, nullptr); }
         | SUB NOT_OBJ                           { $$ = bindNodes($1, $2, nullptr); }
         | ADD error                             { maker->pushTextError("expected primary-expression", @2); YYABORT; }
         | SUB error                             { maker->pushTextError("expected primary-expression", @2); YYABORT; }
         | ASSIGN error                          { maker->pushError("expected primary-expression before token =" , @2); YYABORT; }
         | OR error                              { maker->pushError("expected primary-expression before token ||", @2); YYABORT; }
         | AND error                             { maker->pushError("expected primary-expression before token &&", @2); YYABORT; }
         | EQ error                              { maker->pushError("expected primary-expression before token ==", @2); YYABORT; }
         | NEQ error                             { maker->pushError("expected primary-expression before token !=", @2); YYABORT; }
         | LEQ error                             { maker->pushError("expected primary-expression before token <=", @2); YYABORT; }
         | GEQ error                             { maker->pushError("expected primary-expression before token >=", @2); YYABORT; }
         | STL error                             { maker->pushError("expected primary-expression before token <" , @2); YYABORT; }
         | STG error                             { maker->pushError("expected primary-expression before token >" , @2); YYABORT; }
         | MUL error                             { maker->pushError("expected primary-expression before token *" , @2); YYABORT; }
         | DIV error                             { maker->pushError("expected primary-expression before token /" , @2); YYABORT; }
         | REM error                             { maker->pushError("expected primary-expression before token %" , @2); YYABORT; }
;

NOT_OBJ: NOT OBJ                                 { $$ = std::make_shared<AST>(std::make_shared<OperationNode>(OperationNode($1))); pushBranch($$.get(), $2); }
       | OBJ                                     { $$ = $1; }
;

OBJ: EXPRINBR                                    { $$ = $1; }
   | FUNCTION                                    { $$ = $1; }
   | VAR_DECL                                    { $$ = $1; }
   | VAR                                         { $$ = $1; }
   | VAR_SQBR                                    { $$ = $1; }
   | NUMBER                                      { $$ = $1; }
   | NEW_OBJ                                     { $$ = $1; }
   | STRING                                      { $1.erase($1.begin()); $1.erase(--$1.end()); $$ = std::make_shared<AST>(std::make_shared<StringNode>(StringNode($1))); }
   | SYMBOL                                      { $$ = std::make_shared<AST>(std::make_shared<SymbolNode>(SymbolNode($1))); }
;

EXPRINBR: ORB EXPR CRB                           { $$ = $2; }
        | ORB error                              { maker->pushTextError("expected )", @2); YYABORT; }
;

FUNCTION: WORD_DOT_WORD ORB FPARAMS CRB          { $$ = std::make_shared<AST>(std::make_shared<FunctionNode>(FunctionNode($1))); pushBranches($$.get(), &$3); }
        | WORD_DOT_WORD ORB error                { maker->pushTextError("expected )", @3); YYABORT; }
;

FPARAMS: EXPR COMMA FPARAMS                      { $3.push_front($1); $$ = std::move($3); }
       | EXPR                                    { $$ = std::list<astp>(); $$.push_back($1); }
       | EXPR error                              { maker->pushTextError("expected )", @2); YYABORT; }
       | %empty                                  { }
;

CONTROL: IF_ELIF_ELSE                            { $$ = $1; }
       | WHILE_SCOPE                             { $$ = $1; }
;

IF_ELIF_ELSE: IF_SCOPE ELIF_ELSE                 { $$ = $1; pushBranch($$.get(), $2); }

ELIF_ELSE: ELIF_SCOPE ELIF_ELSE                  { $$ = $1; pushBranch($$.get(), $2); }
         | ELSE_SCOPE                            { $$ = $1; }
         | %empty                                { $$ = nullptr; }
;

IF_SCOPE: IF EXPRINBR SCOPE                      { $$ = std::make_shared<AST>(std::make_shared<ControlNode>(ControlNode($1))); pushBranch($$.get(), $2); pushBranch($$.get(), $3); }
        | IF EXPRINBR error                      { maker->pushTextError("expected {", @3); YYABORT; }
        | IF error                               { maker->pushTextError("expected (", @2); YYABORT; }
;

ELIF_SCOPE: ELIF EXPRINBR SCOPE                  { $$ = std::make_shared<AST>(std::make_shared<ControlNode>(ControlNode($1))); pushBranch($$.get(), $2); pushBranch($$.get(), $3); }
          | ELIF EXPRINBR error                  { maker->pushTextError("expected {", @3); YYABORT; }
          | ELIF error                           { maker->pushTextError("expected (", @2); YYABORT; }
;

ELSE_SCOPE: ELSE SCOPE                           { $$ = std::make_shared<AST>(std::make_shared<ControlNode>(ControlNode($1))); pushBranch($$.get(), $2); }
          | ELSE error                           { maker->pushTextError("expected {", @2); YYABORT; }
;

WHILE_SCOPE: WHILE EXPRINBR SCOPE                { $$ = std::make_shared<AST>(std::make_shared<ControlNode>(ControlNode($1))); pushBranch($$.get(), $2); pushBranch($$.get(), $3); }
           | WHILE EXPRINBR error                { maker->pushTextError("expected {", @3); YYABORT; }
           | WHILE error                         { maker->pushTextError("expected (", @2); YYABORT; }
;

VAR_DECL: TYPE WORD                              { $$ =  std::make_shared<AST>(std::make_shared<VariableDeclarationNode>(VariableDeclarationNode($2, $1))); }
        | TYPE error                             { maker->pushError("expected variable name", @2); YYABORT; }
;

VAR: WORD_DOT_WORD                               { $$ = std::make_shared<AST>(std::make_shared<VariableNode>(VariableNode($1))); }

VAR_SQBR: VAR SQBRS                              { $$ = std::make_shared<AST>(std::make_shared<OperationNode>(OperationNode(OperationType::SQR_BR))); pushBranch($$.get(), $1); pushBranches($$.get(), &$2); }

SQBRS: SQBR SQBRS                                { $2.push_front($1); $$ = std::move($2); }
     | SQBR                                      { $$ = std::list<astp>(); $$.push_back($1); }
;

SQBR: OSB EXPR CSB                               { $$ = $2; }
    | OSB EXPR error                             { maker->pushTextError("expected ]", @3); YYABORT; }
    | OSB error                                  { maker->pushTextError("expected expression", @2); YYABORT; }
;

NUMBER: FLOATNUMBER                              { $$ = std::make_shared<AST>(std::make_shared<NumberNode>(NumberNode($1))); }
      | INTNUMBER                                { $$ = std::make_shared<AST>(std::make_shared<NumberNode>(NumberNode($1))); }
      | FALSE                                    { $$ = std::make_shared<AST>(std::make_shared<NumberNode>(NumberNode(false))); }
      | TRUE                                     { $$ = std::make_shared<AST>(std::make_shared<NumberNode>(NumberNode(true))); }
;

NEW_OBJ: NEW WORD                                { $$ = std::make_shared<AST>(std::make_shared<OperationNode>(OperationNode($1))); $$->emplace_branch(std::make_shared<TypeNode>(TypeNode($2))); }

WORD_DOT_WORD: WORD                              { $$ = std::move($1); }
             | WORD_DOT                          { $$ = std::move($1); }
;

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, yy::parser::location_type* location, ASTMaker* maker)
{
    return maker->yylex(yylval, location);
}

void pushBranch(AST* root, astp br)
{
    if (br)
    {
        root->emplace_branch(std::move(*br));
    }
}

void pushBranches(AST* root, std::list<astp>* list)
{
    for (auto& br : *list)
    {
        if (br)
        {
            root->emplace_branch(std::move(*br));
        }
    }
}

astp bindNodes(OperationType op, astp lhs, astp rhs)
{
    astp node = std::make_shared<AST>(std::make_shared<OperationNode>(OperationNode(op)));

    if (rhs)
    {
        node->emplace_branch(std::move(*rhs));

        (*node)[0].emplace_branch(std::move((*node)[0][0]));
        (*node)[0][0] = std::move(*lhs);
    }
    else
    {
        node->emplace_branch(std::move(*lhs));
    }

    return node;
}

astp bindNodes(astp lhs, astp rhs)
{
    if (rhs)
    {
        rhs->emplace_branch(std::move((*rhs)[0]));
        (*rhs)[0] = std::move(*lhs);
        return rhs;
    }
    return lhs;
}

void parser::error(const parser::location_type&, const std::string&) {}

} // namespace yy