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
    AST ast;                     \
    ast_maker.make(&ast); //

TEST(ASTMakerTest, Empty) // NOLINT
{
    CONSTRUCT_FILE(
        " "
    )

    EXPECT_TRUE(ast.branches_num() == 0);
    EXPECT_TRUE(ast.value().get()->type() == ASTNodeType::ROOT);
}

TEST(ASTMakerTest, EmptyClass) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main;"
    )

    EXPECT_TRUE(ast.branches_num() == 1);
    EXPECT_TRUE(ast.value().get()->type() == ASTNodeType::ROOT);
    EXPECT_TRUE(ast[0].value().get()->type() == ASTNodeType::CLASS);
    EXPECT_TRUE(ast[0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ClassNode*>(ast[0].value().get())->name == "Main");
}

TEST(ASTMakerTest, ClassScope) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {}"
    )

    EXPECT_TRUE(ast.branches_num() == 1);
    EXPECT_TRUE(ast.value().get()->type() == ASTNodeType::ROOT);
    EXPECT_TRUE(ast[0].value().get()->type() == ASTNodeType::CLASS);
    EXPECT_TRUE(ast[0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ClassNode*>(ast[0].value().get())->name == "Main");
}

TEST(ASTMakerTest, ClassOneField) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a;\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->var_type == VariableType::INT);
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

    EXPECT_TRUE(ast[0][0].value().get()->type() == ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][1].value().get()->type() == ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->name == "b1");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][2].value().get()->type() == ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][2].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][2].value().get())->name == "_c");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][2].value().get())->access_type == AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][2].value().get())->var_type == VariableType::CHAR);
}

TEST(ASTMakerTest, ClassOneMethod) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void do_some(int a, float b) {}\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][0].branches_num() == 3);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->name == "do_some");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->modifier == MethodType::STATIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->ret_type == VariableType::VOID);

    EXPECT_TRUE(ast[0][0][0].value().get()->type() == ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][0][1].value().get()->type() == ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][0][2].value().get()->type() == ASTNodeType::SCOPE);
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

    EXPECT_TRUE(ast[0][0].value().get()->type() == ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][0].branches_num() == 3);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->name == "sum");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->access_type == AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->modifier == MethodType::STATIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->ret_type == VariableType::INT);

    EXPECT_TRUE(ast[0][0][0].value().get()->type() == ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][0][1].value().get()->type() == ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][0][2].value().get()->type() == ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][2].branches_num() == 0);

    EXPECT_TRUE(ast[0][1].value().get()->type() == ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][1].branches_num() == 2);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][1].value().get())->name == "print");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][1].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][1].value().get())->modifier == MethodType::NATIVE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][1].value().get())->ret_type == VariableType::VOID);

    EXPECT_TRUE(ast[0][1][0].value().get()->type() == ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][1][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][1][0].value().get())->name == "c");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][1][0].value().get())->var_type == VariableType::CHAR);

    EXPECT_TRUE(ast[0][1][1].value().get()->type() == ASTNodeType::SCOPE);
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

    EXPECT_TRUE(ast[0][0].value().get()->type() == ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][1].value().get()->type() == ASTNodeType::FIELD);
    EXPECT_TRUE(ast[0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][2].value().get()->type() == ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][2].branches_num() == 3);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][2].value().get())->name == "sum");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][2].value().get())->access_type == AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][2].value().get())->modifier == MethodType::INSTANCE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][2].value().get())->ret_type == VariableType::INT);

    EXPECT_TRUE(ast[0][2][0].value().get()->type() == ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][2][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][2][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][2][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][2][1].value().get()->type() == ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][2][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][2][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][2][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][2][2].value().get()->type() == ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][2][2].branches_num() == 0);

    EXPECT_TRUE(ast[0][3].value().get()->type() == ASTNodeType::METHOD);
    EXPECT_TRUE(ast[0][3].branches_num() == 2);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][3].value().get())->name == "print");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][3].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][3].value().get())->modifier == MethodType::NATIVE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][3].value().get())->ret_type == VariableType::VOID);

    EXPECT_TRUE(ast[0][3][0].value().get()->type() == ASTNodeType::MET_PAR);
    EXPECT_TRUE(ast[0][3][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][3][0].value().get())->name == "c");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][3][0].value().get())->var_type == VariableType::CHAR);

    EXPECT_TRUE(ast[0][3][1].value().get()->type() == ASTNodeType::SCOPE);
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

    EXPECT_TRUE(ast[0][1][2][0].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][0].value().get())->op_type == OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][1][2][0][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][0][0].value().get())->name == "sum");

    EXPECT_TRUE(ast[0][1][2][0][1].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][0][1].value().get())->op_type == OperationType::ADD);

    EXPECT_TRUE(ast[0][1][2][0][1][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][0][1][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][0][1][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][0][1][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][1].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][1].value().get())->op_type == OperationType::SUB);

    EXPECT_TRUE(ast[0][1][2][1][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][1][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][1][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][1][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][2].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][2].value().get())->op_type == OperationType::MUL);

    EXPECT_TRUE(ast[0][1][2][2][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][2][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][2][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][2][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][3].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][3].value().get())->op_type == OperationType::DIV);

    EXPECT_TRUE(ast[0][1][2][3][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][3][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][3][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][3][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][4].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][4].value().get())->op_type == OperationType::OR);

    EXPECT_TRUE(ast[0][1][2][4][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][4][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][4][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][4][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][5].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][5].value().get())->op_type == OperationType::AND);

    EXPECT_TRUE(ast[0][1][2][5][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][5][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][5][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][5][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][6].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][6].value().get())->op_type == OperationType::EQ);

    EXPECT_TRUE(ast[0][1][2][6][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][6][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][6][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][6][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][7].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][7].value().get())->op_type == OperationType::NEQ);

    EXPECT_TRUE(ast[0][1][2][7][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][7][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][7][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][7][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][8].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][8].value().get())->op_type == OperationType::LEQ);

    EXPECT_TRUE(ast[0][1][2][8][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][8][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][8][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][8][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][9].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][9].value().get())->op_type == OperationType::GEQ);

    EXPECT_TRUE(ast[0][1][2][9][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][9][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][9][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][9][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][10].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][10].value().get())->op_type == OperationType::STL);

    EXPECT_TRUE(ast[0][1][2][10][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][10][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][10][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][10][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][11].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][11].value().get())->op_type == OperationType::STG);

    EXPECT_TRUE(ast[0][1][2][11][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][11][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][11][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][11][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][12].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][12].value().get())->op_type == OperationType::SHL);

    EXPECT_TRUE(ast[0][1][2][12][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][12][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][12][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][12][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][13].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][13].value().get())->op_type == OperationType::SHR);

    EXPECT_TRUE(ast[0][1][2][13][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][13][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][13][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][13][1].value().get())->name == "right");

    EXPECT_TRUE(ast[0][1][2][14].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][14].value().get())->op_type == OperationType::REM);

    EXPECT_TRUE(ast[0][1][2][14][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][14][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][14][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][14][1].value().get())->name == "right");
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

    EXPECT_TRUE(ast[0][0][1][0].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][1][0].value().get())->op_type == OperationType::RETURN);

    EXPECT_TRUE(ast[0][0][1][0][0].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][1][0][0].value().get())->op_type == OperationType::NOT);

    EXPECT_TRUE(ast[0][0][1][0][0][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][1][0][0][0].value().get())->name == "a");
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

    EXPECT_TRUE(ast[0][0][1][0].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][1][0].value().get())->op_type == OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][1][0][0].value().get()->type() == ASTNodeType::VAR_DECL);
    EXPECT_TRUE(static_cast<VariableDeclarationNode*>(ast[0][0][1][0][0].value().get())->var_type == VariableType::INT);
    EXPECT_TRUE(static_cast<VariableDeclarationNode*>(ast[0][0][1][0][0].value().get())->name == "num");

    EXPECT_TRUE(ast[0][0][1][0][1].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][1][0][1].value().get())->op_type == OperationType::MUL);

    EXPECT_TRUE(ast[0][0][1][0][1][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][1][0][1][0].value().get())->name == "a");

    EXPECT_TRUE(ast[0][0][1][0][1][1].value().get()->type() == ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<FunctionNode*>(ast[0][0][1][0][1][1].value().get())->name == "foo");

    EXPECT_TRUE(ast[0][0][1][0][1][1][0].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][1][0][1][1][0].value().get())->op_type == OperationType::SUB);

    EXPECT_TRUE(ast[0][0][1][0][1][1][0][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][1][0][1][1][0][0].value().get())->name == "a");

    EXPECT_TRUE(ast[0][0][1][0][1][1][0][1].value().get()->type() == ASTNodeType::NUMBER);
    EXPECT_TRUE(static_cast<NumberNode*>(ast[0][0][1][0][1][1][0][1].value().get())->num_type == VariableType::INT);
    EXPECT_TRUE(static_cast<NumberNode*>(ast[0][0][1][0][1][1][0][1].value().get())->number.i == 1);

    EXPECT_TRUE(ast[0][0][1][0][1][1][1].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][1][0][1][1][1].value().get())->name == "a");
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

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == ASTNodeType::CONTROL);
    EXPECT_TRUE(static_cast<ControlNode*>(ast[0][0][0][0].value().get())->control_type == ControlType::IF);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][0][0][0].value().get())->name == "cond1");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][0][0][1][0].value().get()->type() == ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<FunctionNode*>(ast[0][0][0][0][1][0].value().get())->name == "foo1");

    EXPECT_TRUE(ast[0][0][0][0][2].value().get()->type() == ASTNodeType::CONTROL);
    EXPECT_TRUE(static_cast<ControlNode*>(ast[0][0][0][0][2].value().get())->control_type == ControlType::ELIF);

    EXPECT_TRUE(ast[0][0][0][0][2][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][0][0][2][0].value().get())->name == "cond2");

    EXPECT_TRUE(ast[0][0][0][0][2][1].value().get()->type() == ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][0][0][2][1][0].value().get()->type() == ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<FunctionNode*>(ast[0][0][0][0][2][1][0].value().get())->name == "foo2");

    EXPECT_TRUE(ast[0][0][0][0][2][2].value().get()->type() == ASTNodeType::CONTROL);
    EXPECT_TRUE(static_cast<ControlNode*>(ast[0][0][0][0][2][2].value().get())->control_type == ControlType::ELSE);

    EXPECT_TRUE(ast[0][0][0][0][2][2][0].value().get()->type() == ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][0][0][2][2][0][0].value().get()->type() == ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<FunctionNode*>(ast[0][0][0][0][2][2][0][0].value().get())->name == "foo3");
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

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == ASTNodeType::CONTROL);
    EXPECT_TRUE(static_cast<ControlNode*>(ast[0][0][0][0].value().get())->control_type == ControlType::WHILE);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][0][0][0].value().get())->name == "cond");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == ASTNodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][0][0][1][0].value().get()->type() == ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<FunctionNode*>(ast[0][0][0][0][1][0].value().get())->name == "foo");
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

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][0][0].value().get())->op_type == OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][0][0][0].value().get())->name == "str");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == ASTNodeType::STRING);
    EXPECT_TRUE(static_cast<StringNode*>(ast[0][0][0][0][1].value().get())->value == "abcd");
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

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][0][0].value().get())->op_type == OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][0][0][0].value().get())->name == "ch");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == ASTNodeType::SYMBOL);
    EXPECT_TRUE(static_cast<SymbolNode*>(ast[0][0][0][0][1].value().get())->value == 'a');
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

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][0][0].value().get())->op_type == OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][0][0][0].value().get())->name == "str");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][0][0][1].value().get())->op_type == OperationType::NEW);

    EXPECT_TRUE(ast[0][0][0][0][1][0].value().get()->type() == ASTNodeType::TYPE);
    EXPECT_TRUE(static_cast<TypeNode*>(ast[0][0][0][0][1][0].value().get())->str == "String");
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

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == ASTNodeType::FUNCTION);
    EXPECT_TRUE(static_cast<FunctionNode*>(ast[0][0][0][0].value().get())->name == "Other.foo");

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][0][0][0].value().get())->name == "Other.var");
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

    EXPECT_TRUE(ast[0][0][0][0].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][0][0].value().get())->op_type == OperationType::ASSIGN);

    EXPECT_TRUE(ast[0][0][0][0][0].value().get()->type() == ASTNodeType::VAR_DECL);
    EXPECT_TRUE(static_cast<VariableDeclarationNode*>(ast[0][0][0][0][0].value().get())->var_type == VariableType::INT);
    EXPECT_TRUE(static_cast<VariableDeclarationNode*>(ast[0][0][0][0][0].value().get())->name == "a");

    EXPECT_TRUE(ast[0][0][0][0][1].value().get()->type() == ASTNodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][0][0][0][1].value().get())->op_type == OperationType::SQR_BR);

    EXPECT_TRUE(ast[0][0][0][0][1][0].value().get()->type() == ASTNodeType::VARIABLE);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][0][0][0][1][0].value().get())->name == "b");

    EXPECT_TRUE(ast[0][0][0][0][1][1].value().get()->type() == ASTNodeType::NUMBER);
    EXPECT_TRUE(static_cast<NumberNode*>(ast[0][0][0][0][1][1].value().get())->num_type == VariableType::INT);
    EXPECT_TRUE(static_cast<NumberNode*>(ast[0][0][0][0][1][1].value().get())->number.i == 1);

    EXPECT_TRUE(ast[0][0][0][0][1][2].value().get()->type() == ASTNodeType::NUMBER);
    EXPECT_TRUE(static_cast<NumberNode*>(ast[0][0][0][0][1][2].value().get())->num_type == VariableType::INT);
    EXPECT_TRUE(static_cast<NumberNode*>(ast[0][0][0][0][1][2].value().get())->number.i == 2);
}

TEST(ASTMakerTest, Error) // NOLINT
{
    CONSTRUCT_FILE(
        "err\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, ClassError) // NOLINT
{
    CONSTRUCT_FILE(
        "class\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, ClassColonError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, ClassScopeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, AccessError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   err\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, FieldTypeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public err\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, FieldTypeVoidError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public void\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, FieldNameError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, FieldColonError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, MethodTypeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, MethodNameError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, MethodParamsError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, MethodParamsBracketError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo(\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, MethodScopeError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, MethodParamNameError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo(int) {\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
}

TEST(ASTMakerTest, MethodParamsCommaError) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo(int a,) {\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
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

    EXPECT_TRUE(ast_maker.err());
}

#undef CONSTRUCT_FILE