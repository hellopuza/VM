#ifndef INTERPRETER_H_INCLUDED
#define INTERPRETER_H_INCLUDED

class Interpreter
{
public:
    Interpreter() = delete;
    explicit Interpreter(std::string& byte_code_path);
    ~Interpreter() = default;

    static void read_file();

private:
    std::string byte_code_path_;
    std::string byte_code_buffer_;
};

#endif // INTERPRETER_H_INCLUDED