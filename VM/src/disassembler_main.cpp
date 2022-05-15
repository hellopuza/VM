#include "Disassembler/Disassembler.h"

#include <iostream>

#define CHECK_ERROR(cond, message)      \
    if (cond) {                         \
        std::cout << (message) << "\n"; \
        return -1;                      \
    } //

int main(int argc, const char* argv[])
{
    Disassembler dis;
    for (int i = 1; i < argc; i++)
    {
        std::string filename(argv[i]);
        CHECK_ERROR(!dis.disassemble(filename), "File not found: " + filename);
        dis.print(std::cout);
    }

    return 0;
}