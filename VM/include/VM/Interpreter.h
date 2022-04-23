#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include <string>
#include <iostream>

class Interpreter
{
public:
    Interpreter() = default;
    ~Interpreter() = default;

    int interpret_goto();

    static void read_file(const std::string& filename, std::string* out_buffer);

private:
    ClassLinker* cl;
};

#endif // INTERPRETER_H_INCLUDED