#include "Compiler/Translator/Translator.h"
#include "VM/ClassLinker.h"

#include <fstream>
#include <iostream>
#include <string>

#include <gtest/gtest.h> // NOLINT

#define CONSTRUCT_FILE(code)     \
    std::ofstream ofile("file"); \
    ofile << (code);             \
    ofile.close();               \
    std::ifstream ifile("file"); \
    ASTMaker ast_maker(&ifile);  \
    AST ast;                     \
    ast_maker.make(&ast);        \
    ifile.close();               \
    Translator trans(&ast);      \
    ofile.open("file");          \
    trans.translate(&ofile);     \
    ofile.close();               \
    ifile.open("file");          \
    std::stringstream ss;        \
    ss << ifile.rdbuf();         \
    Klasses kls = {ss.str()};    \
    ClassLinker cl;              \
    cl.link(kls); //

TEST(TranslatorTest, EmptyClass) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main;"
    )

    EXPECT_TRUE(cl.classes.size() == 1);
    EXPECT_TRUE(cl.classes.contains("Main"));
}

TEST(TranslatorTest, ClassFields) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public int a;\n"
        "   private float b;\n"
        "}\n"
    )

    EXPECT_TRUE(cl.classes.size() == 1);
    EXPECT_TRUE(cl.classes.contains("Main"));
    EXPECT_TRUE(cl.classes["Main"].fields.size() == 2);
    EXPECT_TRUE(cl.classes["Main"].methods.size() == 0);

    EXPECT_TRUE(cl.classes["Main"].fields.contains("a"));
    EXPECT_TRUE(cl.classes["Main"].fields["a"].access_type == AccessType::PUBLIC);
    EXPECT_TRUE(cl.classes["Main"].fields["a"].var_type == VariableType::INT);

    EXPECT_TRUE(cl.classes["Main"].fields.contains("b"));
    EXPECT_TRUE(cl.classes["Main"].fields["b"].access_type == AccessType::PRIVATE);
    EXPECT_TRUE(cl.classes["Main"].fields["b"].var_type == VariableType::FLOAT);
}

TEST(TranslatorTest, ClassMethods) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int sum(int a, float b) {}\n"
        "   public native void print(long c) {}\n"
        "}\n"
    )

    EXPECT_TRUE(cl.classes.size() == 1);
    EXPECT_TRUE(cl.classes.contains("Main"));
    EXPECT_TRUE(cl.classes["Main"].fields.size() == 0);
    EXPECT_TRUE(cl.classes["Main"].methods.size() == 2);

    EXPECT_TRUE(cl.classes["Main"].methods.contains("sum"));
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].access_type == AccessType::PRIVATE);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].modifier == MethodType::STATIC);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].ret_type == VariableType::INT);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].met_params.size() == 2);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].met_params[0] == VariableType::INT);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].met_params[1] == VariableType::FLOAT);

    EXPECT_TRUE(cl.classes["Main"].methods.contains("print"));
    EXPECT_TRUE(cl.classes["Main"].methods["print"].access_type == AccessType::PUBLIC);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].modifier == MethodType::NATIVE);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].ret_type == VariableType::VOID);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].met_params.size() == 1);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].met_params[0] == VariableType::LONG);
}

TEST(TranslatorTest, MethodScope) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int test() {\n"
        "       int a = 0;\n"
        "   }\n"
        "}\n"
    )
    ast.dot_dump("graph");
}

#undef CONSTRUCT_FILE