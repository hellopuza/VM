#include "VM/PkmVM.h"
#include "Opcodes.h"

#include <fstream>

Interpreter::Interpreter(std::string& byte_code_path) : byte_code_path_(byte_code_path)
{
    read_file(byte_code_path_, &byte_code_buffer_);
}

int Interpreter::interpret_goto()
{
    std::size_t pc = 0;

    static void* dispatch_table[] = {
        &&NOP, 
    };

    #define DISPATCH() goto *dispatch_table[byte_code_buffer_[pc++]]

    while (true)
    {
        NOP:
            DISPATCH();
    }
}

void Interpreter::read_file(const std::string& filename, std::string* out_buffer)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cout << "Error, can't open " << filename << " file" << std::endl;
        return;
    }
    std::getline(file, *out_buffer, '\0');
}