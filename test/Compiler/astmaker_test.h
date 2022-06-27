#include "Compiler/AST/ASTMaker.h"

#include <fstream>
#include <iostream>
#include <string>

#include <gtest/gtest.h> // NOLINT

#define CONSTRUCT_FILE(str)      \
    std::ofstream ofile("file"); \
    ofile << (str);              \
    ofile.close();               \
    std::ifstream ifile("file"); \
    ASTMaker ast_maker(&ifile);  \
    ast::AST ast;                \
    ast_maker.make(&ast); //

TEST(ASTMakerTest, Empty) // NOLINT
{
    CONSTRUCT_FILE(
        " "
    )

    EXPECT_TRUE(ast.branches_num() == 0);
    EXPECT_TRUE(ast.value().get()->type() == pkm::ASTNodeType::ROOT);
}

TEST(ASTMakerTest, EmptyClass) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main;"
    )

    EXPECT_TRUE(ast.branches_num() == 1);
    EXPECT_TRUE(ast.value().get()->type() == pkm::ASTNodeType::ROOT);
    EXPECT_TRUE(ast[0].value().get()->type() == pkm::ASTNodeType::CLASS);
    EXPECT_TRUE(ast[0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::ClassNode*>(ast[0].value().get())->name == "Main");
}

TEST(ASTMakerTest, ClassScope) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {}"
    )

    EXPECT_TRUE(ast.branches_num() == 1);
    EXPECT_TRUE(ast.value().get()->type() == pkm::ASTNodeType::ROOT);
    EXPECT_TRUE(ast[0].value().get()->type() == pkm::ASTNodeType::CLASS);
    EXPECT_TRUE(ast[0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::ClassNode*>(ast[0].value().get())->name == "Main");
}

TEST(ASTMakerTest, ClassOneField) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a;\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == pkm::ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][0].value().get())->access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][0].value().get())->var_type.type == pkm::VariableType::INT);
}

TEST(ASTMakerTest, ClassManyFields) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a;\n"
        "   public float b1;\n"
        "   private char _c;\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == pkm::ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][0].value().get())->access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][0].value().get())->var_type.type == pkm::VariableType::INT);

    EXPECT_TRUE(ast[0][1].value().get()->type() == pkm::ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][1].value().get())->name == "b1");
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][1].value().get())->access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][1].value().get())->var_type.type == pkm::VariableType::FLOAT);

    EXPECT_TRUE(ast[0][2].value().get()->type() == pkm::ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][2].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][2].value().get())->name == "_c");
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][2].value().get())->access_type == pkm::AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][2].value().get())->var_type.type == pkm::VariableType::CHAR);
}

TEST(ASTMakerTest, ClassOneMethod) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void do_some(int a, float b) {}\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == pkm::ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][0].branches_num() == 3);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][0].value().get())->name == "do_some");
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][0].value().get())->access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][0].value().get())->modifier == pkm::MethodType::STATIC);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][0].value().get())->ret_type.type == pkm::VariableType::VOID);

    EXPECT_TRUE(ast[0][0][0].value().get()->type() == pkm::ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][0][0].value().get())->var_type.type == pkm::VariableType::INT);

    EXPECT_TRUE(ast[0][0][1].value().get()->type() == pkm::ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][0][1].value().get())->var_type.type == pkm::VariableType::FLOAT);

    EXPECT_TRUE(ast[0][0][2].value().get()->type() == pkm::ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][2].branches_num() == 0);
}

TEST(ASTMakerTest, ClassManyMethods) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int sum(int a, float b) {}\n"
        "   public native void print(char c) {}\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == pkm::ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][0].branches_num() == 3);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][0].value().get())->name == "sum");
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][0].value().get())->access_type == pkm::AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][0].value().get())->modifier == pkm::MethodType::STATIC);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][0].value().get())->ret_type.type == pkm::VariableType::INT);

    EXPECT_TRUE(ast[0][0][0].value().get()->type() == pkm::ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][0][0].value().get())->var_type.type == pkm::VariableType::INT);

    EXPECT_TRUE(ast[0][0][1].value().get()->type() == pkm::ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][0][1].value().get())->var_type.type == pkm::VariableType::FLOAT);

    EXPECT_TRUE(ast[0][0][2].value().get()->type() == pkm::ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][2].branches_num() == 0);

    EXPECT_TRUE(ast[0][1].value().get()->type() == pkm::ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][1].branches_num() == 2);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][1].value().get())->name == "print");
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][1].value().get())->access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][1].value().get())->modifier == pkm::MethodType::NATIVE);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][1].value().get())->ret_type.type == pkm::VariableType::VOID);

    EXPECT_TRUE(ast[0][1][0].value().get()->type() == pkm::ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][1][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][1][0].value().get())->name == "c");
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][1][0].value().get())->var_type.type == pkm::VariableType::CHAR);

    EXPECT_TRUE(ast[0][1][1].value().get()->type() == pkm::ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][1][1].branches_num() == 0);
}

TEST(ASTMakerTest, ClassManyFieldsMethods) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a;\n"
        "   public float b;\n"
        "   private instance int sum(int a, float b) {}\n"
        "   public native void print(char c) {}\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == pkm::ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][0].value().get())->access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][0].value().get())->var_type.type == pkm::VariableType::INT);

    EXPECT_TRUE(ast[0][1].value().get()->type() == pkm::ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][1].value().get())->access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<ast::FieldNode*>(ast[0][1].value().get())->var_type.type == pkm::VariableType::FLOAT);

    EXPECT_TRUE(ast[0][2].value().get()->type() == pkm::ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][2].branches_num() == 3);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][2].value().get())->name == "sum");
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][2].value().get())->access_type == pkm::AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][2].value().get())->modifier == pkm::MethodType::INSTANCE);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][2].value().get())->ret_type.type == pkm::VariableType::INT);

    EXPECT_TRUE(ast[0][2][0].value().get()->type() == pkm::ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][2][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][2][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][2][0].value().get())->var_type.type == pkm::VariableType::INT);

    EXPECT_TRUE(ast[0][2][1].value().get()->type() == pkm::ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][2][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][2][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][2][1].value().get())->var_type.type == pkm::VariableType::FLOAT);

    EXPECT_TRUE(ast[0][2][2].value().get()->type() == pkm::ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][2][2].branches_num() == 0);

    EXPECT_TRUE(ast[0][3].value().get()->type() == pkm::ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][3].branches_num() == 2);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][3].value().get())->name == "print");
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][3].value().get())->access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][3].value().get())->modifier == pkm::MethodType::NATIVE);
    EXPECT_TRUE(static_cast<ast::MethodNode*>(ast[0][3].value().get())->ret_type.type == pkm::VariableType::VOID);

    EXPECT_TRUE(ast[0][3][0].value().get()->type() == pkm::ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][3][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][3][0].value().get())->name == "c");
    EXPECT_TRUE(static_cast<ast::MethodParameterNode*>(ast[0][3][0].value().get())->var_type.type == pkm::VariableType::CHAR);

    EXPECT_TRUE(ast[0][3][1].value().get()->type() == pkm::ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][3][1].branches_num() == 0);
}

TEST(ASTMakerTest, BinaryOperators) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a;\n"
        "   private static int sum(int left, int rigth) {\n"
        "       sum = left + right;\n"
        "       left - right;\n"
        "       left * right;\n"
        "       left / right;\n"
        "       left || right;\n"
        "       left && right;\n"
        "       left == right;\n"
        "       left != right;\n"
        "       left <= right;\n"
        "       left >= right;\n"
        "       left < right;\n"
        "       left > right;\n"
        "       left << right;\n"
        "       left >> right;\n"
        "       left % right;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][1][2][0].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][0].value().get())->op_type == pkm::OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][1][2][0][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][0][0].value().get())->name == "sum");

    EXPECT_TRUE(ast[0][1][2][0][1].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][0][1].value().get())->op_type == pkm::OperationType::ADD);

    EXPECT_TRUE(ast[0][1][2][0][1][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][0][1][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][0][1][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][0][1][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][1].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][1].value().get())->op_type == pkm::OperationType::SUB);

    EXPECT_TRUE(ast[0][1][2][1][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][1][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][1][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][1][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][2].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][2].value().get())->op_type == pkm::OperationType::MUL);

    EXPECT_TRUE(ast[0][1][2][2][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][2][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][2][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][2][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][3].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][3].value().get())->op_type == pkm::OperationType::DIV);

    EXPECT_TRUE(ast[0][1][2][3][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][3][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][3][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][3][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][4].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][4].value().get())->op_type == pkm::OperationType::OR);

    EXPECT_TRUE(ast[0][1][2][4][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][4][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][4][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][4][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][5].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][5].value().get())->op_type == pkm::OperationType::AND);

    EXPECT_TRUE(ast[0][1][2][5][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][5][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][5][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][5][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][6].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][6].value().get())->op_type == pkm::OperationType::EQ);

    EXPECT_TRUE(ast[0][1][2][6][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][6][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][6][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][6][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][7].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][7].value().get())->op_type == pkm::OperationType::NEQ);

    EXPECT_TRUE(ast[0][1][2][7][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][7][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][7][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][7][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][8].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][8].value().get())->op_type == pkm::OperationType::LEQ);

    EXPECT_TRUE(ast[0][1][2][8][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][8][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][8][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][8][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][9].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][9].value().get())->op_type == pkm::OperationType::GEQ);

    EXPECT_TRUE(ast[0][1][2][9][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][9][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][9][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][9][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][10].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][10].value().get())->op_type == pkm::OperationType::STL);

    EXPECT_TRUE(ast[0][1][2][10][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][10][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][10][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][10][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][11].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][11].value().get())->op_type == pkm::OperationType::STG);

    EXPECT_TRUE(ast[0][1][2][11][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][11][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][11][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][11][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][12].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][12].value().get())->op_type == pkm::OperationType::SHL);

    EXPECT_TRUE(ast[0][1][2][12][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][12][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][12][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][12][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][13].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][13].value().get())->op_type == pkm::OperationType::SHR);

    EXPECT_TRUE(ast[0][1][2][13][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][13][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][13][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][13][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][14].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][1][2][14].value().get())->op_type == pkm::OperationType::REM);

    EXPECT_TRUE(ast[0][1][2][14][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][14][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][14][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][1][2][14][1].value().get())->name == "right");
}

TEST(ASTMakerTest, OperatorNot) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int not(int a) {\n"
        "       return !a;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][1][0].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][1][0].value().get())->op_type == pkm::OperationType::RETURN);

    EXPECT_TRUE(ast[0][0][1][0][0].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][1][0][0].value().get())->op_type == pkm::OperationType::NOT);

    EXPECT_TRUE(ast[0][0][1][0][0][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][1][0][0][0].value().get())->name == "a");
}

TEST(ASTMakerTest, Function) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int main(int a) {\n"
        "       int num = a * foo(a - 1, a);\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][1][0].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][1][0].value().get())->op_type == pkm::OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][1][0][0].value().get()->type() == pkm::ASTNodeType::VAR_DECL);
    EXPECT_TRUE(static_cast<ast::VariableDeclarationNode*>(ast[0][0][1][0][0].value().get())->var_type.type == pkm::VariableType::INT);
    EXPECT_TRUE(static_cast<ast::VariableDeclarationNode*>(ast[0][0][1][0][0].value().get())->name == "num");

    EXPECT_TRUE(ast[0][0][1][0][1].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][1][0][1].value().get())->op_type == pkm::OperationType::MUL);

    EXPECT_TRUE(ast[0][0][1][0][1][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][1][0][1][0].value().get())->name == "a");

    EXPECT_TRUE(ast[0][0][1][0][1][1].value().get()->type() == pkm::ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<ast::FunctionNode*>(ast[0][0][1][0][1][1].value().get())->name == "foo");

    EXPECT_TRUE(ast[0][0][1][0][1][1][0].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][1][0][1][1][0].value().get())->op_type == pkm::OperationType::SUB);

    EXPECT_TRUE(ast[0][0][1][0][1][1][0][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][1][0][1][1][0][0].value().get())->name == "a");

    EXPECT_TRUE(ast[0][0][1][0][1][1][0][1].value().get()->type() == pkm::ASTNodeType::NUMBER);
    EXPECT_TRUE(static_cast<ast::NumberNode*>(ast[0][0][1][0][1][1][0][1].value().get())->num_type == pkm::VariableType::INT);
    EXPECT_TRUE(static_cast<ast::NumberNode*>(ast[0][0][1][0][1][1][0][1].value().get())->number.i == 1);

    EXPECT_TRUE(ast[0][0][1][0][1][1][1].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][1][0][1][1][1].value().get())->name == "a");
}

TEST(ASTMakerTest, ControlIfElifElse) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int main() {\n"
        "       if (cond1) {\n"
        "           foo1();\n"
        "       } elif (cond2) {\n"
        "           foo2();\n"
        "       } else {\n"
        "           foo3();\n"
        "       }\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == pkm::ASTNodeType::CONTROL);
    EXPECT_TRUE(static_cast<ast::ControlNode*>(ast[0][0][0][0].value().get())->control_type == pkm::ControlType::IF);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][0][0][0].value().get())->name == "cond1");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == pkm::ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][0][0][1][0].value().get()->type() == pkm::ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<ast::FunctionNode*>(ast[0][0][0][0][1][0].value().get())->name == "foo1");

    EXPECT_TRUE(ast[0][0][0][0][2].value().get()->type() == pkm::ASTNodeType::CONTROL);
    EXPECT_TRUE(static_cast<ast::ControlNode*>(ast[0][0][0][0][2].value().get())->control_type == pkm::ControlType::ELIF);

    EXPECT_TRUE(ast[0][0][0][0][2][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][0][0][2][0].value().get())->name == "cond2");

    EXPECT_TRUE(ast[0][0][0][0][2][1].value().get()->type() == pkm::ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][0][0][2][1][0].value().get()->type() == pkm::ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<ast::FunctionNode*>(ast[0][0][0][0][2][1][0].value().get())->name == "foo2");

    EXPECT_TRUE(ast[0][0][0][0][2][2].value().get()->type() == pkm::ASTNodeType::CONTROL);
    EXPECT_TRUE(static_cast<ast::ControlNode*>(ast[0][0][0][0][2][2].value().get())->control_type == pkm::ControlType::ELSE);

    EXPECT_TRUE(ast[0][0][0][0][2][2][0].value().get()->type() == pkm::ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][0][0][2][2][0][0].value().get()->type() == pkm::ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<ast::FunctionNode*>(ast[0][0][0][0][2][2][0][0].value().get())->name == "foo3");
}

TEST(ASTMakerTest, ControlWhile) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int main() {\n"
        "       while (cond) {\n"
        "           foo();\n"
        "       }\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == pkm::ASTNodeType::CONTROL);
    EXPECT_TRUE(static_cast<ast::ControlNode*>(ast[0][0][0][0].value().get())->control_type == pkm::ControlType::WHILE);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][0][0][0].value().get())->name == "cond");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == pkm::ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][0][0][1][0].value().get()->type() == pkm::ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<ast::FunctionNode*>(ast[0][0][0][0][1][0].value().get())->name == "foo");
}

TEST(ASTMakerTest, String) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int main() {\n"
        "       str = \"abcd\";\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][0][0].value().get())->op_type == pkm::OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][0][0][0].value().get())->name == "str");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == pkm::ASTNodeType::STRING);
    EXPECT_TRUE(static_cast<ast::StringNode*>(ast[0][0][0][0][1].value().get())->value == "abcd");
}

TEST(ASTMakerTest, Symbol) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int main() {\n"
        "       ch = \'a\';\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][0][0].value().get())->op_type == pkm::OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][0][0][0].value().get())->name == "ch");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == pkm::ASTNodeType::SYMBOL);
    EXPECT_TRUE(static_cast<ast::SymbolNode*>(ast[0][0][0][0][1].value().get())->value == 'a');
}

TEST(ASTMakerTest, OperatorNew) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int main() {\n"
        "       str = new String;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][0][0].value().get())->op_type == pkm::OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][0][0][0].value().get())->name == "str");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][0][0][1].value().get())->op_type == pkm::OperationType::NEW);

    EXPECT_TRUE(ast[0][0][0][0][1][0].value().get()->type() == pkm::ASTNodeType::TYPE);
    EXPECT_TRUE(static_cast<ast::TypeNode*>(ast[0][0][0][0][1][0].value().get())->data_type.type == pkm::VariableType::REFERENCE);
    EXPECT_TRUE(static_cast<ast::TypeNode*>(ast[0][0][0][0][1][0].value().get())->data_type.name == "String");
}

TEST(ASTMakerTest, DotWord) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int main() {\n"
        "       Other.foo(Other.var);\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == pkm::ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<ast::FunctionNode*>(ast[0][0][0][0].value().get())->name == "Other.foo");

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][0][0][0].value().get())->name == "Other.var");
}

TEST(ASTMakerTest, SquareBrackets) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int main() {\n"
        "       int a = b[1][2];\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][0][0].value().get())->op_type == pkm::OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == pkm::ASTNodeType::VAR_DECL);
    EXPECT_TRUE(static_cast<ast::VariableDeclarationNode*>(ast[0][0][0][0][0].value().get())->var_type.type == pkm::VariableType::INT);
    EXPECT_TRUE(static_cast<ast::VariableDeclarationNode*>(ast[0][0][0][0][0].value().get())->name == "a");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == pkm::ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<ast::OperationNode*>(ast[0][0][0][0][1].value().get())->op_type == pkm::OperationType::SQR_BR);

    EXPECT_TRUE(ast[0][0][0][0][1][0].value().get()->type() == pkm::ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<ast::VariableNode*>(ast[0][0][0][0][1][0].value().get())->name == "b");

    EXPECT_TRUE(ast[0][0][0][0][1][1].value().get()->type() == pkm::ASTNodeType::NUMBER);
    EXPECT_TRUE(static_cast<ast::NumberNode*>(ast[0][0][0][0][1][1].value().get())->num_type == pkm::VariableType::INT);
    EXPECT_TRUE(static_cast<ast::NumberNode*>(ast[0][0][0][0][1][1].value().get())->number.i == 1);

    EXPECT_TRUE(ast[0][0][0][0][1][2].value().get()->type() == pkm::ASTNodeType::NUMBER);
    EXPECT_TRUE(static_cast<ast::NumberNode*>(ast[0][0][0][0][1][2].value().get())->num_type == pkm::VariableType::INT);
    EXPECT_TRUE(static_cast<ast::NumberNode*>(ast[0][0][0][0][1][2].value().get())->number.i == 2);
}

TEST(ASTMakerTest, VariableDeclaration) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int main() {\n"
        "       int a;\n"
        "       String str;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == pkm::ASTNodeType::VAR_DECL);
    EXPECT_TRUE(static_cast<ast::VariableDeclarationNode*>(ast[0][0][0][0].value().get())->var_type.type == pkm::VariableType::INT);
    EXPECT_TRUE(static_cast<ast::VariableDeclarationNode*>(ast[0][0][0][0].value().get())->name == "a");

    EXPECT_TRUE(ast[0][0][0][1].value().get()->type() == pkm::ASTNodeType::VAR_DECL);
    EXPECT_TRUE(static_cast<ast::VariableDeclarationNode*>(ast[0][0][0][1].value().get())->var_type.type == pkm::VariableType::REFERENCE);
    EXPECT_TRUE(static_cast<ast::VariableDeclarationNode*>(ast[0][0][0][1].value().get())->var_type.name == "String");
    EXPECT_TRUE(static_cast<ast::VariableDeclarationNode*>(ast[0][0][0][1].value().get())->name == "str");
}

TEST(ASTMakerTest, Error) // NOLINT
{
    CONSTRUCT_FILE(
        "err\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CLASS);
}

TEST(ASTMakerTest, ClassError) // NOLINT
{
    CONSTRUCT_FILE(
        "class\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CLASS_NAME);
}

TEST(ASTMakerTest, ClassColonError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_COLON);
}

TEST(ASTMakerTest, ClassScopeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CCB);
}

TEST(ASTMakerTest, AccessError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   err\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CCB);
}

TEST(ASTMakerTest, FieldTypeVoidError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public void\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_FIELD_TYPE);
}

TEST(ASTMakerTest, FieldNameError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_FIELD_NAME);
}

TEST(ASTMakerTest, FieldColonError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_COLON);
}

TEST(ASTMakerTest, MethodTypeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_METHOD_RETURN_TYPE);
}

TEST(ASTMakerTest, MethodNameError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_METHOD_NAME);
}

TEST(ASTMakerTest, MethodParamsError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_ORB);
}

TEST(ASTMakerTest, MethodParamsBracketError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo(\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CRB);
}

TEST(ASTMakerTest, MethodScopeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CCB);
}

TEST(ASTMakerTest, MethodParamNameError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo(int) {\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_METHOD_PARAMETER_NAME);
}

TEST(ASTMakerTest, MethodParamsCommaError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo(int a,) {\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CRB);
}

TEST(ASTMakerTest, ReturnColonError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       return 1\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_COLON);
}

TEST(ASTMakerTest, ReturnError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       return\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_COLON);
}

TEST(ASTMakerTest, ColonError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       foo()\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_COLON);
}

TEST(ASTMakerTest, AssignExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a = ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, OrExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a || ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, AndExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a && ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, EqExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a == ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, NeqExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a != ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, StlExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a < ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, StgExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a > ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, LeqExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a <= ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, GeqExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a >= ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, AddExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a + ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, SubExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a - ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, MulExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a * ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, DivExprError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       a / ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION);
}

TEST(ASTMakerTest, ExprInBracketsError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       (foo();\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CRB);
}

TEST(ASTMakerTest, FunctionOpenBracketError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       foo);\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_COLON);
}

TEST(ASTMakerTest, FunctionCloseBracketError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       foo(a;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CRB);
}

TEST(ASTMakerTest, IfScopeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       if (cond) }\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_OCB);
}

TEST(ASTMakerTest, IfCondError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       if (cond {}\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CRB);
}

TEST(ASTMakerTest, ElifError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       elif (cond) {}\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CCB);
}

TEST(ASTMakerTest, ElifScopeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       if (cond) {foo()} elif (cond) }\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_COLON);
}

TEST(ASTMakerTest, ElifCondError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       if (cond) {foo()} elif (cond {}\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_COLON);
}

TEST(ASTMakerTest, ElseError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       else {}\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CCB);
}

TEST(ASTMakerTest, ElseScopeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       if (cond) {foo()} elif (cond) {} else }\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_COLON);
}

TEST(ASTMakerTest, VarDeclError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       int ;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CCB);
}

TEST(ASTMakerTest, SquareBracketsError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       int a = b[;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_EXPRESSION);
}

TEST(ASTMakerTest, SquareBracketsCloseError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       int a = b[a;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.getError().type == CompilationError::Type::EXPECTED_CSB);
}

#undef CONSTRUCT_FILE