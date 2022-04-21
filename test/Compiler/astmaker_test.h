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
    EXPECT_TRUE(ast.value().get()->type() == NodeType::ROOT);
}

TEST(ASTMakerTest, EmptyClass) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main;"
    )

    EXPECT_TRUE(ast.branches_num() == 1);
    EXPECT_TRUE(ast.value().get()->type() == NodeType::ROOT);
    EXPECT_TRUE(ast[0].value().get()->type() == NodeType::CLASS);
    EXPECT_TRUE(ast[0].branches_num() == 0);
    EXPECT_TRUE(static_cast<ClassNode*>(ast[0].value().get())->name == "Main");
}

TEST(ASTMakerTest, ClassScope) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {}"
    )

    EXPECT_TRUE(ast.branches_num() == 1);
    EXPECT_TRUE(ast.value().get()->type() == NodeType::ROOT);
    EXPECT_TRUE(ast[0].value().get()->type() == NodeType::CLASS);
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

    EXPECT_TRUE(ast[0][0].value().get()->type() == NodeType::FIELD);
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
        "   public float b;\n"
        "\n"
        "   private byte _c;\n"
        "   private float d;\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == NodeType::FIELD);
    EXPECT_TRUE(ast[0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][1].value().get()->type() == NodeType::FIELD);
    EXPECT_TRUE(ast[0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][2].value().get()->type() == NodeType::FIELD);
    EXPECT_TRUE(ast[0][2].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][2].value().get())->name == "_c");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][2].value().get())->access_type == AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][2].value().get())->var_type == VariableType::BYTE);

    EXPECT_TRUE(ast[0][3].value().get()->type() == NodeType::FIELD);
    EXPECT_TRUE(ast[0][3].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][3].value().get())->name == "d");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][3].value().get())->access_type == AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][3].value().get())->var_type == VariableType::FLOAT);
}

TEST(ASTMakerTest, ClassOneMethod) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void do_some(int a, float b) {}\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == NodeType::METHOD);
    EXPECT_TRUE(ast[0][0].branches_num() == 3);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->name == "do_some");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->modifier == MethodType::STATIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->ret_type == VariableType::VOID);

    EXPECT_TRUE(ast[0][0][0].value().get()->type() == NodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][0][1].value().get()->type() == NodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][0][2].value().get()->type() == NodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][2].branches_num() == 0);
}

TEST(ASTMakerTest, ClassManyMethods) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int sum(int a, float b) {}\n"
        "   public native void print(long c) {}\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][0].value().get()->type() == NodeType::METHOD);
    EXPECT_TRUE(ast[0][0].branches_num() == 3);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->name == "sum");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->access_type == AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->modifier == MethodType::STATIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][0].value().get())->ret_type == VariableType::INT);

    EXPECT_TRUE(ast[0][0][0].value().get()->type() == NodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][0][1].value().get()->type() == NodeType::MET_PAR);
    EXPECT_TRUE(ast[0][0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][0][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][0][2].value().get()->type() == NodeType::SCOPE);
    EXPECT_TRUE(ast[0][0][2].branches_num() == 0);

    EXPECT_TRUE(ast[0][1].value().get()->type() == NodeType::METHOD);
    EXPECT_TRUE(ast[0][1].branches_num() == 2);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][1].value().get())->name == "print");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][1].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][1].value().get())->modifier == MethodType::NATIVE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][1].value().get())->ret_type == VariableType::VOID);

    EXPECT_TRUE(ast[0][1][0].value().get()->type() == NodeType::MET_PAR);
    EXPECT_TRUE(ast[0][1][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][1][0].value().get())->name == "c");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][1][0].value().get())->var_type == VariableType::LONG);

    EXPECT_TRUE(ast[0][1][1].value().get()->type() == NodeType::SCOPE);
    EXPECT_TRUE(ast[0][1][1].branches_num() == 0);
}

TEST(ASTMakerTest, ClassManyFieldsMethods) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a;\n"
        "   public float b;\n"
        "   private instance int sum(int a, float b) {}\n"
        "   public native void print(long c) {}\n"
        "}\n"
    )
    ast.dot_dump("graph");

    EXPECT_TRUE(ast[0][0].value().get()->type() == NodeType::FIELD);
    EXPECT_TRUE(ast[0][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][1].value().get()->type() == NodeType::FIELD);
    EXPECT_TRUE(ast[0][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<FieldNode*>(ast[0][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][2].value().get()->type() == NodeType::METHOD);
    EXPECT_TRUE(ast[0][2].branches_num() == 3);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][2].value().get())->name == "sum");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][2].value().get())->access_type == AccessType::PRIVATE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][2].value().get())->modifier == MethodType::INSTANCE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][2].value().get())->ret_type == VariableType::INT);

    EXPECT_TRUE(ast[0][2][0].value().get()->type() == NodeType::MET_PAR);
    EXPECT_TRUE(ast[0][2][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][2][0].value().get())->name == "a");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][2][0].value().get())->var_type == VariableType::INT);

    EXPECT_TRUE(ast[0][2][1].value().get()->type() == NodeType::MET_PAR);
    EXPECT_TRUE(ast[0][2][1].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][2][1].value().get())->name == "b");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][2][1].value().get())->var_type == VariableType::FLOAT);

    EXPECT_TRUE(ast[0][2][2].value().get()->type() == NodeType::SCOPE);
    EXPECT_TRUE(ast[0][2][2].branches_num() == 0);

    EXPECT_TRUE(ast[0][3].value().get()->type() == NodeType::METHOD);
    EXPECT_TRUE(ast[0][3].branches_num() == 2);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][3].value().get())->name == "print");
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][3].value().get())->access_type == AccessType::PUBLIC);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][3].value().get())->modifier == MethodType::NATIVE);
    EXPECT_TRUE(static_cast<MethodNode*>(ast[0][3].value().get())->ret_type == VariableType::VOID);

    EXPECT_TRUE(ast[0][3][0].value().get()->type() == NodeType::MET_PAR);
    EXPECT_TRUE(ast[0][3][0].branches_num() == 0);
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][3][0].value().get())->name == "c");
    EXPECT_TRUE(static_cast<MethodParameterNode*>(ast[0][3][0].value().get())->var_type == VariableType::LONG);

    EXPECT_TRUE(ast[0][3][1].value().get()->type() == NodeType::SCOPE);
    EXPECT_TRUE(ast[0][3][1].branches_num() == 0);
}

TEST(ASTMakerTest, MethodScopeOperators) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a;\n"
        "   private static int sum(int left, int rigth) {\n"
        "       return left + right;\n"
        "   }\n"
        "}\n"
    )

    EXPECT_TRUE(ast[0][1][2][0].value().get()->type() == NodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][0].value().get())->op_type == OperationType::RETURN);

    EXPECT_TRUE(ast[0][1][2][0][0].value().get()->type() == NodeType::OPERATION);
    EXPECT_TRUE(static_cast<OperationNode*>(ast[0][1][2][0][0].value().get())->op_type == OperationType::ADD);

    EXPECT_TRUE(ast[0][1][2][0][0][0].value().get()->type() == NodeType::VAR);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][0][0][0].value().get())->name == "left");

    EXPECT_TRUE(ast[0][1][2][0][0][1].value().get()->type() == NodeType::VAR);
    EXPECT_TRUE(static_cast<VariableNode*>(ast[0][1][2][0][0][1].value().get())->name == "right");
}

#undef CONSTRUCT_FILE