#include "Compiler/Translator/Translator.h"
#include "Opcodes.h"
#include "VM/ClassLinker.h"

#include <fstream>
#include <iostream>
#include <string>

#include <gtest/gtest.h> // NOLINT

#define CONSTRUCT_FILE(code)                \
    std::ofstream ofile("file");            \
    ofile << (code);                        \
    ofile.close();                          \
    std::ifstream ifile("file");            \
    ASTMaker ast_maker(&ifile);             \
    AST ast;                                \
    ast_maker.make(&ast);                   \
    ifile.close();                          \
    auto* cls = static_cast<AST*>(&ast[0]); \
    Translator trans(cls);                  \
    ofile.open("file");                     \
    trans.translate(&ofile);                \
    ofile.close();                          \
    ifile.open("file");                     \
    std::stringstream ss;                   \
    ss << ifile.rdbuf();                    \
    Klasses kls = {ss.str()};               \
    ClassLinker cl;                         \
    cl.link(kls); //

#define CHECK_INSTR(instr) \
    EXPECT_TRUE(*reinterpret_cast<uint32_t*>(&cl.classes["Main"].bytecode[pos]) == (instr)); \
    pos += 4; //

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
        "   public native void print(char c) {}\n"
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
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].name == 0);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].locals_num == 2);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].offset == 0x0);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].met_params.size() == 2);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].met_params[0] == VariableType::INT);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].met_params[1] == VariableType::FLOAT);

    EXPECT_TRUE(cl.classes["Main"].methods.contains("print"));
    EXPECT_TRUE(cl.classes["Main"].methods["print"].access_type == AccessType::PUBLIC);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].modifier == MethodType::NATIVE);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].ret_type == VariableType::VOID);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].name == 1);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].locals_num == 1);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].offset == 0x4);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].met_params.size() == 1);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].met_params[0] == VariableType::CHAR);
}

TEST(TranslatorTest, VariableIntDeclaration) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 0;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, VariableFloatDeclaration) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       float a = 0.0;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorReturnInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static int foo() {\n"
        "       return 1;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IRETURN));
}

TEST(TranslatorTest, OperatorReturnFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static float foo() {\n"
        "       return 1.2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FRETURN));
}

TEST(TranslatorTest, OperatorOr) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 || 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IF) + (static_cast<uint16_t>(0x10) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IFN) + (static_cast<uint16_t>(0x18) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::GOTO) + (static_cast<uint16_t>(0x1C) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorAnd) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 && 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IFN) + (static_cast<uint16_t>(0x18) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IFN) + (static_cast<uint16_t>(0x18) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::GOTO) + (static_cast<uint16_t>(0x1C) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorEqualInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 == 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IEQ));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorEqualFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1.2 == 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FEQ));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorNotEqualInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 != 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::INEQ));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorNotEqualFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1.2 != 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FNEQ));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorLessEqualInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 <= 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ILEQ));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorLessEqualFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1.2 <= 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FLEQ));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorGreaterEqualInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 >= 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IGEQ));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorGreaterEqualFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1.2 >= 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FGEQ));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorLessInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 < 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTL));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorLessFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1.2 < 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSTL));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorGreaterInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 > 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTG));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorGreaterFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1.2 > 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSTG));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorShiftLeft) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 << 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISHL));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorShiftRight) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 >> 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISHR));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorAddInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 + 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IADD));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorAddFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       float a = 1.2 + 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FADD));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorSubInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 - 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISUB));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorSubFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       float a = 1.2 - 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSUB));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorMulInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 * 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IMUL));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorMulFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       float a = 1.2 * 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FMUL));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorDivInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 / 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IDIV));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorDivFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       float a = 1.2 / 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FDIV));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorRemainderInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = 1 % 2;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IREM));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorRemainderFloat) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       float a = 1.2 % 2.3;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FREM));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorNot) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       int a = !1;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IFN) + (static_cast<uint16_t>(0x10) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::GOTO) + (static_cast<uint16_t>(0x14) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, FunctionAndControl) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       print(fact(5));\n"
        "   }\n"
        "\n"
        "   public static int fact(int a) {\n"
        "       if (a < 2) {\n"
        "           return 1;\n"
        "       }\n"
        "       return fact(a - 1) * a;\n"
        "   }\n"
        "   public static void print() {}\n"
        "}\n"
    )

    EXPECT_TRUE(cl.classes.size() == 1);
    EXPECT_TRUE(cl.classes.contains("Main"));
    EXPECT_TRUE(cl.classes["Main"].fields.size() == 0);
    EXPECT_TRUE(cl.classes["Main"].methods.size() == 3);

    EXPECT_TRUE(cl.classes["Main"].methods.contains("main"));
    EXPECT_TRUE(cl.classes["Main"].methods["main"].access_type == AccessType::PUBLIC);
    EXPECT_TRUE(cl.classes["Main"].methods["main"].modifier == MethodType::STATIC);
    EXPECT_TRUE(cl.classes["Main"].methods["main"].ret_type == VariableType::VOID);
    EXPECT_TRUE(cl.classes["Main"].methods["main"].name == 0);
    EXPECT_TRUE(cl.classes["Main"].methods["main"].locals_num == 0);
    EXPECT_TRUE(cl.classes["Main"].methods["main"].offset == 0);
    EXPECT_TRUE(cl.classes["Main"].methods["main"].met_params.size() == 0);

    EXPECT_TRUE(cl.classes["Main"].methods.contains("fact"));
    EXPECT_TRUE(cl.classes["Main"].methods["fact"].access_type == AccessType::PUBLIC);
    EXPECT_TRUE(cl.classes["Main"].methods["fact"].modifier == MethodType::STATIC);
    EXPECT_TRUE(cl.classes["Main"].methods["fact"].ret_type == VariableType::INT);
    EXPECT_TRUE(cl.classes["Main"].methods["fact"].name == 4);
    EXPECT_TRUE(cl.classes["Main"].methods["fact"].locals_num == 1);
    EXPECT_TRUE(cl.classes["Main"].methods["fact"].offset == 16);
    EXPECT_TRUE(cl.classes["Main"].methods["fact"].met_params.size() == 1);
    EXPECT_TRUE(cl.classes["Main"].methods["fact"].met_params[0] == VariableType::INT);

    EXPECT_TRUE(cl.classes["Main"].methods.contains("print"));
    EXPECT_TRUE(cl.classes["Main"].methods["print"].access_type == AccessType::PUBLIC);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].modifier == MethodType::STATIC);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].ret_type == VariableType::VOID);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].name == 7);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].locals_num == 0);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].offset == 72);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].met_params.size() == 0);

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(1) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::INVOKESTATIC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::INVOKESTATIC) + (static_cast<uint16_t>(3) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(5) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ILOAD));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTL));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IFN) + (static_cast<uint16_t>(40) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(6) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IRETURN));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ILOAD));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(6) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ILOAD));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISUB));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::INVOKESTATIC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IMUL));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IRETURN));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

#undef CHECK_INSTR
#undef CONSTRUCT_FILE