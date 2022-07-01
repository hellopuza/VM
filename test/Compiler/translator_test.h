#include "Compiler/Analyzer/Analyzer.h"
#include "Compiler/Translator/Translator.h"
#include "Opcodes.h"
#include "VM/ClassLinker.h"

#include <fstream>
#include <iostream>
#include <string>

#include <gtest/gtest.h> // NOLINT

#define CONSTRUCT_FILE(code)                     \
    std::ofstream ofile("file");                 \
    ofile << (code);                             \
    ofile.close();                               \
    std::ifstream ifile("file");                 \
    ASTMaker ast_maker(&ifile);                  \
    ast::AST ast;                                \
    ast_maker.make(&ast);                        \
    ifile.close();                               \
    auto* cls = static_cast<ast::AST*>(&ast[0]); \
    Analyzer anal;                               \
    anal.loadClass(cls);                         \
    anal.analyze(cls);                           \
    Translator trans(cls);                       \
    ofile.open("file");                          \
    trans.translate(&ofile);                     \
    ofile.close();                               \
    ifile.open("file");                          \
    std::stringstream ss;                        \
    ss << ifile.rdbuf();                         \
    Klasses kls = {ss.str()};                    \
    ClassLinker cl;                              \
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
        "   private String c;\n"
        "}\n"
    )

    EXPECT_TRUE(cl.classes.size() == 1);
    EXPECT_TRUE(cl.classes.contains("Main"));
    EXPECT_TRUE(cl.classes["Main"].fields.size() == 3);
    EXPECT_TRUE(cl.classes["Main"].methods.size() == 0);

    EXPECT_TRUE(cl.classes["Main"].fields.contains("a"));
    EXPECT_TRUE(cl.classes["Main"].fields["a"].access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(cl.classes["Main"].fields["a"].var_type.type == pkm::VariableType::INT);
    EXPECT_TRUE(cl.classes["Main"].fields["a"].name == "a");

    EXPECT_TRUE(cl.classes["Main"].fields.contains("b"));
    EXPECT_TRUE(cl.classes["Main"].fields["b"].access_type == pkm::AccessType::PRIVATE);
    EXPECT_TRUE(cl.classes["Main"].fields["b"].var_type.type == pkm::VariableType::FLOAT);
    EXPECT_TRUE(cl.classes["Main"].fields["b"].name == "b");

    EXPECT_TRUE(cl.classes["Main"].fields.contains("c"));
    EXPECT_TRUE(cl.classes["Main"].fields["c"].access_type == pkm::AccessType::PRIVATE);
    EXPECT_TRUE(cl.classes["Main"].fields["c"].var_type.type == pkm::VariableType::REFERENCE);
    EXPECT_TRUE(cl.classes["Main"].fields["c"].var_type.name == "String");
    EXPECT_TRUE(cl.classes["Main"].fields["c"].name == "c");
}

TEST(TranslatorTest, ClassMethods) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   private static int sum(int a, float b) {}\n"
        "   public native void print(String c) {}\n"
        "}\n"
    )

    EXPECT_TRUE(cl.classes.size() == 1);
    EXPECT_TRUE(cl.classes.contains("Main"));
    EXPECT_TRUE(cl.classes["Main"].fields.size() == 0);
    EXPECT_TRUE(cl.classes["Main"].methods.size() == 2);

    EXPECT_TRUE(cl.classes["Main"].methods.contains("sum"));
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].access_type == pkm::AccessType::PRIVATE);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].modifier == pkm::MethodType::STATIC);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].ret_type.type == pkm::VariableType::INT);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].name == "sum");
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].locals_num == 2);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].offset == 0x0);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].met_params.size() == 2);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].met_params[0].type == pkm::VariableType::INT);
    EXPECT_TRUE(cl.classes["Main"].methods["sum"].met_params[1].type == pkm::VariableType::FLOAT);

    EXPECT_TRUE(cl.classes["Main"].methods.contains("print"));
    EXPECT_TRUE(cl.classes["Main"].methods["print"].access_type == pkm::AccessType::PUBLIC);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].modifier == pkm::MethodType::NATIVE);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].ret_type.type == pkm::VariableType::VOID);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].name == "print");
    EXPECT_TRUE(cl.classes["Main"].methods["print"].locals_num == 1);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].offset == 0x4);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].met_params.size() == 1);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].met_params[0].type == pkm::VariableType::REFERENCE);
    EXPECT_TRUE(cl.classes["Main"].methods["print"].met_params[0].name == "String");
}

TEST(TranslatorTest, VariableCharDeclaration) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       char a = 'a';\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FSTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, VariableReferenceDeclaration) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void main() {\n"
        "       Main a = new Main;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::NEW) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ASTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

TEST(TranslatorTest, OperatorReturnChar) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static char foo() {\n"
        "       return 'a';\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IRETURN));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IRETURN));
}

TEST(TranslatorTest, OperatorReturnCharInt) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static int foo() {\n"
        "       return 'a';\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IRETURN));
}

TEST(TranslatorTest, OperatorReturnIntChar) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static char foo() {\n"
        "       return 1;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::I2C));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::FRETURN));
}

TEST(TranslatorTest, OperatorReturnReference) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static Main foo() {\n"
        "       return new Main;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::NEW));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ARETURN));
}

TEST(TranslatorTest, OperatorReturn) // NOLINT
{
    CONSTRUCT_FILE(
        "class Main {\n"
        "   public static void foo() {\n"
        "       return;\n"
        "   }\n"
        "}\n"
    )

    size_t pos = 0;
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IF) + (static_cast<uint16_t>(0x10) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IFN) + (static_cast<uint16_t>(0x18) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::GOTO) + (static_cast<uint16_t>(0x1C) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(4) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IFN) + (static_cast<uint16_t>(0x18) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IFN) + (static_cast<uint16_t>(0x18) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::GOTO) + (static_cast<uint16_t>(0x1C) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(4) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
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
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::IFN) + (static_cast<uint16_t>(0x10) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(3) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::GOTO) + (static_cast<uint16_t>(0x14) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::LDC) + (static_cast<uint16_t>(2) << 0x10));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::ISTORE));
    CHECK_INSTR(static_cast<uint8_t>(Opcode::RETURN));
}

#undef CHECK_INSTR
#undef CONSTRUCT_FILE