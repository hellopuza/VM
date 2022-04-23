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
    explicit Interpreter(PNIEnv* PNIEnv) : pni_env(PNIEnv) {};
    ~Interpreter() = default;

    int start_interpreting(pclass cls, pmethodID mid);

    static void read_file(const std::string& filename, std::string* out_buffer);

private:
    PNIEnv* pni_env;
};

#endif // INTERPRETER_H_INCLUDED