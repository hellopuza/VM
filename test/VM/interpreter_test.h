#ifndef INTERPRETER_TEST_INCLUDED
#define INTERPRETER_TEST_INCLUDED

#include "VM/Interpreter.h"
#include "VM/ClassLinker.h"

#include <memory>
#include <fstream>

#include <gtest/gtest.h> // NOLINT

struct TestInterpreter
{
    TestInterpreter();

    void fillInClass();
    void fillInBytecode();

    std::unique_ptr<PkmVM> vm;
    std::unique_ptr<Interpreter> interpreter;
    std::unique_ptr<PkmClasses> classes; // using PkmClasses = std::unordered_map<std::string, PkmClass>;
};

TestInterpreter::TestInterpreter()
{
    vm          = std::make_unique<PkmVM>();
    classes     = std::make_unique<PkmClasses>();
    interpreter = std::make_unique<Interpreter>(vm.get(), classes.get());
}

TEST(InterpreterTests, TestLDC) // NOLINT
{
    TestInterpreter test;
    PkmClass test_class;

    test.classes.get()->emplace(std::make_pair("test_class", test_class));
}

#endif // INTERPRETER_TEST_INCLUDED
