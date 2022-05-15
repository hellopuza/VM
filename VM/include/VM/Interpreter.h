#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <iostream>

#include "VM/ClassLinker.h"
#include "VM/PkmVM.h"

class PNIEnv;

class Interpreter
{
public:
    Interpreter(PkmVM* pvm);
    void start_interpreting(pclass cls, pmethodID mid);
    
private:
    PkmVM* pvm_;
};

#endif // INTERPRETER_H