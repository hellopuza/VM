#ifndef COMPILER_TRANSLATOR_TRANSLATOR_H
#define COMPILER_TRANSLATOR_TRANSLATOR_H

#include "Compiler/AST/AST.h"

#include <fstream>

class Translator
{
public:
    Translator(AST* ast);

    void translate(std::fstream* file);

private:
    static void writeFields(AST* class_node, std::fstream* file);
    static void writeMethods(AST* class_node, std::fstream* file);

    AST* ast_;
};

#endif // COMPILER_TRANSLATOR_TRANSLATOR_H