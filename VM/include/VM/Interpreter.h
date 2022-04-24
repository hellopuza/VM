#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include <string>
#include <iostream>

#include "VM/Pkm/PkmMethod.h"
#include "VM/Pkm/PkmClass.h"
#include "VM/Pkm/PkmField.h"

class PNIEnv;

class Interpreter
{
public:
    Interpreter(PkmVM* pvm, PkmClasses* pclasses) : pvm_(pvm), pclasses_(pclasses) {};
    ~Interpreter() = default;

    void start_interpreting(pclass cls, pmethodID mid);

    static void read_file(const std::string& filename, std::string* out_buffer);

private:
    PkmVM* pvm_ = nullptr;
    PkmClasses* pclasses_ = nullptr;
};

#endif // INTERPRETER_H_INCLUDED