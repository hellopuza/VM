#include "Compiler/Compiler.h"

#include <filesystem>
#include <iostream>
#include <vector>

const char* const LANG_EXTENSION = ".pkm";
const char* const CODE_EXTENSION = ".klass";
const char* const LIB_FOLDER = "./pkm/lang";

int main(int argc, const char* argv[])
{
    Compiler comp(LIB_FOLDER);
    for (int i = 1; i < argc; i++)
    {
        std::string filename(argv[i]);
        std::filesystem::path path(filename);
        std::string ext(path.extension());
        if (ext != LANG_EXTENSION)
        {
            std::cout << "Wrong extension: " + filename + "\nRequired: " + LANG_EXTENSION + "\n";
            return -1;
        }

        comp.load(filename);
        if (comp.getError().type != CompilationError::Type::OK)
        {
            comp.printErrors(std::cout);
            return -1;
        }
    }

    comp.compile(CODE_EXTENSION);
    if (comp.getError().type != CompilationError::Type::OK)
    {
        comp.printErrors(std::cout);
        return -1;
    }

    return 0;
}