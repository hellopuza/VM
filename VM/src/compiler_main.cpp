#include "Compiler/Compiler.h"

#include <filesystem>
#include <iostream>
#include <vector>

#define PRINT_ERROR(message) \
    std::cout << (message) << "\n"; \
    return -1;

const char* const LANG_EXTENSION = ".pkm";
const char* const CODE_EXTENSION = ".klass";

int main(int argc, char* argv[])
{
    Compiler comp;
    for (int i = 1; i < argc; i++)
    {
        std::string filename(argv[i]);
        std::filesystem::path path(filename);
        std::string ext(path.extension());
        if (ext != LANG_EXTENSION)
        {
            PRINT_ERROR("Wrong extension: " + filename + "\nRequired: " + LANG_EXTENSION);
        }
        if (!comp.load(filename))
        {
            PRINT_ERROR("File not found: " + filename);
        }
        if (!comp.compile(path.parent_path().string() + path.stem().string() + CODE_EXTENSION))
        {
            PRINT_ERROR("File not compiled: " + filename);
        }
    }

    return 0;
}