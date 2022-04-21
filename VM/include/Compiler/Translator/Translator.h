#ifndef COMPILER_TRANSLATOR_TRANSLATOR_H
#define COMPILER_TRANSLATOR_TRANSLATOR_H

#include "Compiler/AST/AST.h"
#include "ConstantPool.h"

#include <fstream>
#include <sstream>

class Translator
{
public:
    Translator(AST* ast);

    void translate(std::ofstream* file);

private:
    void writeConstantPool(std::ofstream* file);
    void writeFields(AST* class_node, std::stringstream* ss);
    void writeMethods(AST* class_node, std::stringstream* ss);
    static void writeMethodParams(AST* method_node, std::stringstream* ss);

    AST* ast_;
    ConstantPool const_pool_;
};

#endif // COMPILER_TRANSLATOR_TRANSLATOR_H