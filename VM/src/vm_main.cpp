#include "VM/VM.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#define PRINT_ERROR(message) \
    std::cout << (message) << "\n"; \
    return -1;

const char* const BIN_FOLDER = "./pkm/bin";

int main(int argc, char* argv[])
{
    for (const auto& entry : std::filesystem::directory_iterator(BIN_FOLDER))
    {
        std::cout << entry.path() << "\n";
    }
    for (int i = 1; i < argc; i++)
    {
        std::cout << argv[i] << "\n";
    }

    return 0;
}