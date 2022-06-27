#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <iostream>

#include "VM/PkmVM.h"

class PNIEnv;

class Interpreter
{
public:
    Interpreter(PkmVM* pvm, PkmClasses* classes);
    void start_interpreting(pclass cls, pmethodID mid);
    bool run_system(pmethodID pmet, Frame* current_frame);

private:
    PkmVM* pvm_;
    PkmClasses* classes_;
};

#endif // INTERPRETER_H