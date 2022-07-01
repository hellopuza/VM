#ifndef INTERPRETER_TEST_INCLUDED
#define INTERPRETER_TEST_INCLUDED

#include "VM/Interpreter.h"
#include "VM/ClassLinker.h"

#include <memory>
#include <fstream>

#include <gtest/gtest.h> // NOLINT

class TestInterpreter
{
public:
    TestInterpreter(PkmClass test_class);

    void fillInClass(PkmClasses& classes);
    void fillInBytecode();

private:
    std::unique_ptr<Interpreter> interpreter_ptr_;
    std::unique_ptr<ClassLinker> class_linker_ptr_;
    std::unique_ptr<PkmClass> test_class_ptr_;

    std::unique_ptr<PkmVM> pvm_;
    std::string bytecode_ = "test_bytecode.klass";
};

TestInterpreter::TestInterpreter(PkmClass test_class)
{
    pvm_ = std::make_unique<PkmVM>();

    interpreter_ptr_  = std::make_unique<Interpreter>(pvm_.get(), classes_.get());
    class_linker_ptr_ = std::make_unique<ClassLinker>();
    test_class_ptr_   = std::make_unique<PkmClass>();

    std::ofstream {bytecode_};
}

void TestInterpreter::fillInClass(PkmClass& class)
{
    
}

void TestInterpreter::fillInBytecode()
{

}

TEST(InterpreterTests, TestLDC) // NOLINT
{
    PkmClass test_class;
    fill_in_class(&test_class);

    TestInterpreter(test_class);
}

#endif // INTERPRETER_TEST_INCLUDED
