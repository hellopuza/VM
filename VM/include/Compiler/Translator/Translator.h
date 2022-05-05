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
    void writeFields(AST* class_node, std::stringstream* class_content);
    void writeMethods(AST* class_node, std::stringstream* class_content, std::stringstream* instructions);
    void writeMethodParams(AST* method_node, std::stringstream* class_content);

    void appendLocal(VariableDeclarationNode* var_decl_node);
    uint32_t writeInstructions(AST* scope_node, std::stringstream* instructions);
    VariableType writeObject(AST* obj_node, std::stringstream* instructions);
    VariableType writeOperation(AST* op_node, std::stringstream* instructions);
    VariableType writeFunction(AST* func_node, std::stringstream* instructions);
    VariableType writeNumber(NumberNode* num_node, std::stringstream* instructions);
    VariableType writeLoad(const std::string& name, std::stringstream* instructions);
    static void writeControl(AST* scope_node, size_t ind, std::stringstream* instructions);
    void writeStore(const std::string& name, std::stringstream* instructions);

    AST* ast_;
    ConstantPool const_pool_;
    std::unordered_map<std::string, std::pair<uint16_t, VariableType>> locals_;
};

#endif // COMPILER_TRANSLATOR_TRANSLATOR_H