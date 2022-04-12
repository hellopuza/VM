#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

#include <string>
#include <iostream>

class Interpreter
{
public:
    Interpreter() = delete;
    explicit Interpreter(std::string& byte_code_path);
    ~Interpreter() = default;

    int interpret_goto();

    static void read_file(const std::string& filename, std::string* out_buffer);

private:
    std::string byte_code_path_;
    std::string byte_code_buffer_;
};

#endif // INTERPRETER_H_INCLUDED