#ifndef COMPILER_TRANSLATOR_TRANSLATOR_H
#define COMPILER_TRANSLATOR_TRANSLATOR_H

#include "Compiler/AST/AST.h"
#include "ConstantPool.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

using ConstantPool = std::unordered_map<std::unique_ptr<cp::AbstractType>, uint16_t>;

class Translator
{
public:
    Translator(ast::AST* class_tree);

    void translate(std::ofstream* output_file);

private:
    void writeConstantPool(std::ofstream* output_file);
    void writeFields(ast::AST* class_node, std::stringstream* class_content);
    void writeMethods(ast::AST* class_node, std::stringstream* class_content, std::stringstream* instructions);
    void writeMethodParams(ast::AST* method_node, std::stringstream* class_content);

    void appendLocal(ast::VariableDeclarationNode* var_decl_node);
    uint32_t writeInstructions(ast::AST* scope_node, std::stringstream* instructions);
    void writeObject(ast::AST* obj_node, std::stringstream* instructions);
    void writeOperation(ast::AST* op_node, std::stringstream* instructions, bool in_scope = false);
    void writeConversion(ast::AST* conv_node, std::stringstream* instructions);
    void writeVariable(ast::AST* var_node, std::stringstream* instructions, bool loading);
    void writeFunction(ast::AST* func_node, std::stringstream* instructions, bool in_scope = false);
    void writeNumber(ast::NumberNode* num_node, std::stringstream* instructions);
    void writeString(ast::StringNode* str_node, std::stringstream* instructions);
    void writeSymbol(ast::SymbolNode* sym_node, std::stringstream* instructions);
    void writeLoad(const std::string& name, std::stringstream* instructions);
    void writeStore(const std::string& name, std::stringstream* instructions);
    void writeControl(ast::AST* control_node, std::stringstream* instructions);
    void writeCommands(void* array, std::stringstream* instructions);
    void writeIfElifElse(ast::AST* if_node, std::stringstream* instructions);
    void writeWhile(ast::AST* while_node, std::stringstream* instructions);

    ast::AST* class_tree_;
    ConstantPool const_pool_;
    std::unordered_map<std::string, std::pair<uint16_t, pkm::DataType>> locals_;
};

#endif // COMPILER_TRANSLATOR_TRANSLATOR_H