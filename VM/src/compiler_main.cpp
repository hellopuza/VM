#include "Compiler/Compiler.h"

#include <filesystem>
#include <iostream>
#include <vector>

#define CHECK_ERROR(cond, message)      \
    if (cond) {                         \
        std::cout << (message) << "\n"; \
        return -1;                      \
    } //

const char* const LANG_EXTENSION = ".pkm";
const char* const CODE_EXTENSION = ".klass";

int main(int argc, const char* argv[])
{
    Compiler comp;
    for (int i = 1; i < argc; i++)
    {
        std::string filename(argv[i]);
        std::filesystem::path path(filename);
        std::string ext(path.extension());
        CHECK_ERROR((ext != LANG_EXTENSION), "Wrong extension: " + filename + "\nRequired: " + LANG_EXTENSION);

        int err = comp.load(filename);
        CHECK_ERROR((err == Compiler::FILE_NOT_FOUND), "File not found: " + filename);
        CHECK_ERROR((err == Compiler::FILE_NOT_FOUND), "File not load: " + filename + ", Wrong syntax or empty");
        CHECK_ERROR((!comp.compile(CODE_EXTENSION)), "File not compiled: " + filename);
    }

    return 0;
}