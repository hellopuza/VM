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

using astp = std::shared_ptr<ast::AST>;

class ASTMaker;
}

%code
{
#include "Compiler/AST/ASTMaker.h"

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, yy::parser::location_type* location, ASTMaker* maker);

void pushBranch(ast::AST* root, astp br);
void pushBranches(ast::AST* root, std::list<astp>* list);

astp bindNodes(pkm::OperationType op, astp lhs, astp rhs = nullptr);
astp bindNodes(astp lhs, astp rhs);

void setLocation(astp ast, const yy::location& location);
void setLocation(ast::AST* ast, const yy::location& location);

}
}

%locations

%token

    <pkm::ASTNodeType>  CLASS
    <pkm::AccessType>   PRIVATE
    <pkm::AccessType>   PUBLIC
    <pkm::MethodType>   INSTANCE
    <pkm::MethodType>   STATIC
    <pkm::MethodType>   NATIVE
    <pkm::VariableType> VOID
    <pkm::VariableType> CHAR
    <pkm::VariableType> INT
    <pkm::VariableType> FLOAT

    <pkm::OperationType> RETURN
    <pkm::OperationType> ASSIGN
    <pkm::OperationType> OR
    <pkm::OperationType> AND
    <pkm::OperationType> EQ
    <pkm::OperationType> NEQ
    <pkm::OperationType> LEQ
    <pkm::OperationType> GEQ
    <pkm::OperationType> STL
    <pkm::OperationType> STG
    <pkm::OperationType> SHL
    <pkm::OperationType> SHR
    <pkm::OperationType> ADD
    <pkm::OperationType> SUB
    <pkm::OperationType> MUL
    <pkm::OperationType> DIV
    <pkm::OperationType> REM
    <pkm::OperationType> NOT
    <pkm::OperationType> NEW

    <pkm::ControlType> IF
    <pkm::ControlType> ELSE
    <pkm::ControlType> ELIF
    <pkm::ControlType> WHILE

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

    <pkm::AccessType> ACC
    <pkm::DataType>   TYPE
    <pkm::DataType>   MTYPE
    <pkm::MethodType> MET

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

program: CLASS_DECLS                             { maker->ast()->value() = std::make_shared<ast::ASNode>(ast::ASNode()); }

CLASS_DECLS: CLASS_DECL CLASS_DECLS              { pushBranch(maker->ast(), $1); }
           | %empty                              { }
;

CLASS_DECL: CLASS WORD CLASS_SCOPE               { $$ = std::make_shared<ast::AST>(std::make_shared<ast::ClassNode>(ast::ClassNode($2))); pushBranches($$.get(), &$3); setLocation($$, @2); }
          | CLASS WORD error                     { maker->pushError(CompilationError::Type::EXPECTED_COLON, @3); YYABORT; }
          | CLASS error                          { maker->pushError(CompilationError::Type::EXPECTED_CLASS_NAME, @2); YYABORT; }
          | error                                { maker->pushError(CompilationError::Type::EXPECTED_CLASS, @1); YYABORT; }
;

CLASS_SCOPE: OCB CLASS_FMS CCB                   { $$ = std::move($2); }
           | SCOLON                              { }
           | OCB CLASS_FMS error                 { maker->pushError(CompilationError::Type::EXPECTED_CCB, @3); YYABORT; }
;

CLASS_FMS: CLASS_FM CLASS_FMS                    { $2.push_front($1); $$ = std::move($2); }
         | %empty                                { }
;

CLASS_FM: FIELD                                  { $$ = $1; }
        | METHOD                                 { $$ = $1; }
;

FIELD: ACC TYPE WORD SCOLON                      { $$ = std::make_shared<ast::AST>(std::make_shared<ast::FieldNode>(ast::FieldNode($3, $1, $2))); setLocation($$, @3); }
     | ACC TYPE WORD error                       { maker->pushError(CompilationError::Type::EXPECTED_COLON, @4); YYABORT; }
     | ACC TYPE error                            { maker->pushError(CompilationError::Type::EXPECTED_FIELD_NAME, @3); YYABORT; }
     | ACC error                                 { maker->pushError(CompilationError::Type::EXPECTED_FIELD_TYPE, @2); YYABORT; }
;

METHOD: ACC MET MTYPE WORD MPARAMS SCOPE         { $$ = std::make_shared<ast::AST>(std::make_shared<ast::MethodNode>(ast::MethodNode($4, $1, $2, $3))); pushBranches($$.get(), &$5); pushBranch($$.get(), $6); setLocation($$, @4); }
      | ACC MET MTYPE WORD MPARAMS error         { maker->pushError(CompilationError::Type::EXPECTED_OCB, @6); YYABORT; }
      | ACC MET MTYPE WORD error                 { maker->pushError(CompilationError::Type::EXPECTED_ORB, @5); YYABORT; }
      | ACC MET MTYPE error                      { maker->pushError(CompilationError::Type::EXPECTED_METHOD_NAME, @4); YYABORT; }
      | ACC MET error                            { maker->pushError(CompilationError::Type::EXPECTED_METHOD_RETURN_TYPE, @3); YYABORT; }
;

ACC: PUBLIC                                      { $$ = $1; }
   | PRIVATE                                     { $$ = $1; }
;

TYPE: CHAR                                       { $$ = pkm::DataType($1); }
    | INT                                        { $$ = pkm::DataType($1); }
    | FLOAT                                      { $$ = pkm::DataType($1); }
    | WORD                                       { $$ = pkm::DataType(pkm::VariableType::REFERENCE, $1); }
;

MTYPE: TYPE                                      { $$ = $1; }
     | VOID                                      { $$ = pkm::DataType($1); }
;

MET: INSTANCE                                    { $$ = $1; }
   | STATIC                                      { $$ = $1; }
   | NATIVE                                      { $$ = $1; }
;

MPARAMS: ORB PARAMS CRB                          { $$ = std::move($2); }
       | ORB CRB                                 { }
       | ORB error                               { maker->pushError(CompilationError::Type::EXPECTED_CRB, @2); YYABORT; }
;

PARAMS: PARAM COMMA PARAMS                       { $3.push_front($1); $$ = std::move($3); }
      | PARAM                                    { $$ = std::list<astp>(); $$.push_back($1); }
      | PARAM error                              { maker->pushError(CompilationError::Type::EXPECTED_CRB, @2); YYABORT; }
;

PARAM: TYPE WORD                                 { $$ = std::make_shared<ast::AST>(std::make_shared<ast::MethodParameterNode>(ast::MethodParameterNode($2, $1))); setLocation($$, @2); }
     | TYPE error                                { maker->pushError(CompilationError::Type::EXPECTED_METHOD_PARAMETER_NAME, @2); YYABORT; }
;

SCOPE: OCB ACTIONS CCB                           { $$ = std::make_shared<ast::AST>(std::make_shared<ast::ScopeNode>(ast::ScopeNode())); pushBranches($$.get(), &$2); setLocation($$, @2); }
     | OCB error                                 { maker->pushError(CompilationError::Type::EXPECTED_CCB, @2); YYABORT; }
;

ACTIONS: ACTION ACTIONS                          { $2.push_front($1); $$ = std::move($2); }
       | %empty                                  { }
;

ACTION: EXPR SCOLON                              { $$ = $1; }
      | CONTROL                                  { $$ = $1; }
      | RETURN EXPR SCOLON                       { $$ = std::make_shared<ast::AST>(std::make_shared<ast::OperationNode>(ast::OperationNode($1))); pushBranch($$.get(), $2); setLocation($$, @1); }
      | RETURN SCOLON                            { $$ = std::make_shared<ast::AST>(std::make_shared<ast::OperationNode>(ast::OperationNode($1))); setLocation($$, @1); }
      | RETURN EXPR error                        { maker->pushError(CompilationError::Type::EXPECTED_COLON, @3); YYABORT; }
      | RETURN error                             { maker->pushError(CompilationError::Type::EXPECTED_COLON, @2); YYABORT; }
      | EXPR error                               { maker->pushError(CompilationError::Type::EXPECTED_COLON, @2); YYABORT; }
;

EXPR: CHECK_ASS ASSIGNMENT                       { $$ = bindNodes($1, $2); }

ASSIGNMENT: ASSIGN CHECK_ASS ASSIGNMENT          { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
          | ASSIGN error                         { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
          | %empty                               { $$ = nullptr; }
;

CHECK_ASS: CHECK_OR OP_OR                        { $$ = bindNodes($1, $2); }

OP_OR: OR CHECK_OR OP_OR                         { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
     | OR error                                  { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
     | %empty                                    { $$ = nullptr; }
;

CHECK_OR: CHECK_AND OP_AND                       { $$ = bindNodes($1, $2); }

OP_AND: AND CHECK_AND OP_AND                     { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
      | AND error                                { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
      | %empty                                   { $$ = nullptr; }
;

CHECK_AND: CHECK_EQ OP_EQ                        { $$ = bindNodes($1, $2); }

OP_EQ: EQ CHECK_EQ OP_EQ                         { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
     | NEQ CHECK_EQ OP_EQ                        { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
     | EQ error                                  { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
     | NEQ error                                 { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
     | %empty                                    { $$ = nullptr; }
;

CHECK_EQ: CHECK_COMP OP_COMP                     { $$ = bindNodes($1, $2); }

OP_COMP: STL CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
       | STG CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
       | LEQ CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
       | GEQ CHECK_COMP OP_COMP                  { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
       | STL error                               { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
       | STG error                               { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
       | LEQ error                               { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
       | GEQ error                               { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
       | %empty                                  { $$ = nullptr; }
;

CHECK_COMP: CHECK_SHIFT OP_SHIFT                 { $$ = bindNodes($1, $2); }

OP_SHIFT: SHL CHECK_SHIFT OP_SHIFT               { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
        | SHR CHECK_SHIFT OP_SHIFT               { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
        | SHL error                              { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
        | SHR error                              { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
        | %empty                                 { $$ = nullptr; }
;

CHECK_SHIFT: CHECK_ADD OP_ADD                    { $$ = bindNodes($1, $2); }

OP_ADD: ADD CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
      | SUB CHECK_ADD OP_ADD                     { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
      | ADD error                                { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
      | SUB error                                { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
      | %empty                                   { $$ = nullptr; }
;

CHECK_ADD: CHECK_MUL OP_MUL                      { $$ = bindNodes($1, $2); }

OP_MUL: MUL CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
      | DIV CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
      | REM CHECK_MUL OP_MUL                     { $$ = bindNodes($1, $2, $3); setLocation($$, @1); }
      | MUL error                                { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
      | DIV error                                { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
      | REM error                                { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
      | %empty                                   { $$ = nullptr; }
;

CHECK_MUL: NOT_OBJ                               { $$ = $1; }
         | ADD NOT_OBJ                           { $$ = bindNodes($1, $2, nullptr); setLocation($$, @1); }
         | SUB NOT_OBJ                           { $$ = bindNodes($1, $2, nullptr); setLocation($$, @1); }
         | ADD error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | SUB error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | ASSIGN error                          { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | OR error                              { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | AND error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | EQ error                              { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | NEQ error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | LEQ error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | GEQ error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | STL error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | STG error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | MUL error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | DIV error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
         | REM error                             { maker->pushError(CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION, @2); YYABORT; }
;

NOT_OBJ: NOT OBJ                                 { $$ = std::make_shared<ast::AST>(std::make_shared<ast::OperationNode>(ast::OperationNode($1))); pushBranch($$.get(), $2); setLocation($$, @1); }
       | OBJ                                     { $$ = $1; }
;

OBJ: EXPRINBR                                    { $$ = $1; }
   | FUNCTION                                    { $$ = $1; }
   | VAR_DECL                                    { $$ = $1; }
   | VAR                                         { $$ = $1; }
   | VAR_SQBR                                    { $$ = $1; }
   | NUMBER                                      { $$ = $1; }
   | NEW_OBJ                                     { $$ = $1; }
   | STRING                                      { $1.erase($1.begin()); $1.erase(--$1.end()); $$ = std::make_shared<ast::AST>(std::make_shared<ast::StringNode>(ast::StringNode($1))); setLocation($$, @1); }
   | SYMBOL                                      { $$ = std::make_shared<ast::AST>(std::make_shared<ast::SymbolNode>(ast::SymbolNode($1))); setLocation($$, @1); }
   | THIS                                        { $$ = std::make_shared<ast::AST>(std::make_shared<ast::VariableNode>(ast::VariableNode($1))); setLocation($$, @1); }
;

EXPRINBR: ORB EXPR CRB                           { $$ = $2; }
        | ORB error                              { maker->pushError(CompilationError::Type::EXPECTED_CRB, @2); YYABORT; }
;

FUNCTION: WORD_DOT_WORD ORB FPARAMS CRB          { $$ = std::make_shared<ast::AST>(std::make_shared<ast::FunctionNode>(ast::FunctionNode($1))); pushBranches($$.get(), &$3); setLocation($$, @1); }
        | WORD_DOT_WORD ORB error                { maker->pushError(CompilationError::Type::EXPECTED_CRB, @3); YYABORT; }
;

FPARAMS: EXPR COMMA FPARAMS                      { $3.push_front($1); $$ = std::move($3); }
       | EXPR                                    { $$ = std::list<astp>(); $$.push_back($1); }
       | EXPR error                              { maker->pushError(CompilationError::Type::EXPECTED_CRB, @2); YYABORT; }
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

IF_SCOPE: IF EXPRINBR SCOPE                      { $$ = std::make_shared<ast::AST>(std::make_shared<ast::ControlNode>(ast::ControlNode($1))); pushBranch($$.get(), $2); pushBranch($$.get(), $3); setLocation($$, @2); }
        | IF EXPRINBR error                      { maker->pushError(CompilationError::Type::EXPECTED_OCB, @3); YYABORT; }
        | IF error                               { maker->pushError(CompilationError::Type::EXPECTED_CCB, @2); YYABORT; }
;

ELIF_SCOPE: ELIF EXPRINBR SCOPE                  { $$ = std::make_shared<ast::AST>(std::make_shared<ast::ControlNode>(ast::ControlNode($1))); pushBranch($$.get(), $2); pushBranch($$.get(), $3); setLocation($$, @2); }
          | ELIF EXPRINBR error                  { maker->pushError(CompilationError::Type::EXPECTED_OCB, @3); YYABORT; }
          | ELIF error                           { maker->pushError(CompilationError::Type::EXPECTED_CCB, @2); YYABORT; }
;

ELSE_SCOPE: ELSE SCOPE                           { $$ = std::make_shared<ast::AST>(std::make_shared<ast::ControlNode>(ast::ControlNode($1))); pushBranch($$.get(), $2); setLocation($$, @1); }
          | ELSE error                           { maker->pushError(CompilationError::Type::EXPECTED_OCB, @2); YYABORT; }
;

WHILE_SCOPE: WHILE EXPRINBR SCOPE                { $$ = std::make_shared<ast::AST>(std::make_shared<ast::ControlNode>(ast::ControlNode($1))); pushBranch($$.get(), $2); pushBranch($$.get(), $3); setLocation($$, @2); }
           | WHILE EXPRINBR error                { maker->pushError(CompilationError::Type::EXPECTED_OCB, @3); YYABORT; }
           | WHILE error                         { maker->pushError(CompilationError::Type::EXPECTED_CCB, @2); YYABORT; }
;

VAR_DECL: TYPE WORD                              { $$ = std::make_shared<ast::AST>(std::make_shared<ast::VariableDeclarationNode>(ast::VariableDeclarationNode($2, $1))); setLocation($$, @2); }

VAR: WORD_DOT_WORD                               { $$ = std::make_shared<ast::AST>(std::make_shared<ast::VariableNode>(ast::VariableNode($1))); setLocation($$, @1); }

VAR_SQBR: VAR SQBRS                              { $$ = std::make_shared<ast::AST>(std::make_shared<ast::OperationNode>(ast::OperationNode(pkm::OperationType::SQR_BR))); pushBranch($$.get(), $1); pushBranches($$.get(), &$2); setLocation($$, @1); }

SQBRS: SQBR SQBRS                                { $2.push_front($1); $$ = std::move($2); }
     | SQBR                                      { $$ = std::list<astp>(); $$.push_back($1); }
;

SQBR: OSB EXPR CSB                               { $$ = $2; }
    | OSB EXPR error                             { maker->pushError(CompilationError::Type::EXPECTED_CSB, @3); YYABORT; }
    | OSB error                                  { maker->pushError(CompilationError::Type::EXPECTED_EXPRESSION, @2); YYABORT; }
;

NUMBER: FLOATNUMBER                              { $$ = std::make_shared<ast::AST>(std::make_shared<ast::NumberNode>(ast::NumberNode($1))); setLocation($$, @1); }
      | INTNUMBER                                { $$ = std::make_shared<ast::AST>(std::make_shared<ast::NumberNode>(ast::NumberNode($1))); setLocation($$, @1); }
      | FALSE                                    { $$ = std::make_shared<ast::AST>(std::make_shared<ast::NumberNode>(ast::NumberNode(0))); setLocation($$, @1); }
      | TRUE                                     { $$ = std::make_shared<ast::AST>(std::make_shared<ast::NumberNode>(ast::NumberNode(1))); setLocation($$, @1); }
;

NEW_OBJ: NEW TYPE                                { $$ = std::make_shared<ast::AST>(std::make_shared<ast::OperationNode>(ast::OperationNode($1))); $$->emplace_branch(std::make_shared<ast::TypeNode>(ast::TypeNode($2))); setLocation($$, @1); setLocation(static_cast<ast::AST*>(&((*$$)[0])), @2); }

WORD_DOT_WORD: WORD                              { $$ = std::move($1); }
             | WORD_DOT                          { $$ = std::move($1); }
;

%%

namespace yy {

parser::token_type yylex(parser::semantic_type* yylval, yy::parser::location_type* location, ASTMaker* maker)
{
    return maker->yylex(yylval, location);
}

void pushBranch(ast::AST* root, astp br)
{
    if (br)
    {
        root->emplace_branch(std::move(*br));
    }
}

void pushBranches(ast::AST* root, std::list<astp>* list)
{
    for (auto& br : *list)
    {
        if (br)
        {
            root->emplace_branch(std::move(*br));
        }
    }
}

astp bindNodes(pkm::OperationType op, astp lhs, astp rhs)
{
    astp node = std::make_shared<ast::AST>(std::make_shared<ast::OperationNode>(ast::OperationNode(op)));

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

void setLocation(astp ast, const yy::location& location)
{
    ast->value()->location.begin = location.begin.column;
    ast->value()->location.end = location.end.column;
    ast->value()->location.line = location.begin.line;
}

void setLocation(ast::AST* ast, const yy::location& location)
{
    ast->value()->location.begin = location.begin.column;
    ast->value()->location.end = location.end.column;
    ast->value()->location.line = location.begin.line;
}

void parser::error(const parser::location_type&, const std::string&) {}

} // namespace yy