#include "Compiler/Translator/Translator.h"
#include "Opcodes.h"

#include <algorithm>

Translator::Translator(AST* ast) : ast_(ast) {}

void Translator::translate(std::fstream* file)
{
    auto* class_node = static_cast<AST*>(&((*ast_)[0]));
    std::string class_name = static_cast<ClassNode*>(class_node->value().get())->name;
    file->write(class_name.c_str(), class_name.length());

    writeFields(class_node, file);
    writeMethods(class_node, file);
}

void Translator::writeFields(AST* class_node, std::fstream* file)
{

}

void Translator::writeMethods(AST* class_node, std::fstream* file)
{

}