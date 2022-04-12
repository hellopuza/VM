#include "VM/PkmVM.h"
#include "Opcodes.h"

Interpreter::Interpreter(std::string& byte_code_path) : byte_code_path_(byte_code_path)
{
    file_buffer_ = read_file(byte_code_path_);
}

int Interpreter::interpret_goto()
{
    std::size_t pc = 0;

    static void* dispatch_table[] = {
        &&NOP, 
    }

    #define DISPATCH() goto *dispatch_table[file_buffer_[pc++]];

    while (true)
    {
        NOP:
            DISPATCH();
        default:
            return -1;
    }
}

std::string Interpreter::read_file(std::string& filename)
{
    std::string buffer;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cout << "Error, can't open " << filename << " file" << std::endl;
        return;
    }
    std::getline(file, *buffer, '\0');

    return buffer;
}