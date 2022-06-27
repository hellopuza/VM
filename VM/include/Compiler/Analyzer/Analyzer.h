#ifndef COMPILER_ANALYZER_ANALYZER_H
#define COMPILER_ANALYZER_ANALYZER_H

#include "Compiler/AST/AST.h"
#include "Compiler/CompilationError.h"

#include <unordered_map>
#include <unordered_set>

class Analyzer
{
    struct Field
    {
        pkm::AccessType access_type;
        pkm::DataType var_type;
    };
    using Fields = std::unordered_map<std::string, Field>;

    struct Method
    {
        pkm::AccessType access_type;
        pkm::MethodType modifier;
        pkm::DataType ret_type;
        std::vector<pkm::DataType> met_params;
    };
    using Methods = std::unordered_map<std::string, Method>;

    struct Class
    {
        Fields fields;
        Methods methods;
    };
    using Classes = std::unordered_map<std::string, Class>;

public:
    Analyzer();
    Analyzer(const char* lib_folder);

    void analyze(ast::AST* class_tree);
    void loadClass(const ast::AST* class_tree);
    CompilationError getError() const;

private:
    void analyzeField(ast::AST* field_node);
    void analyzeMethod(ast::AST* met_node);
    void analyzeScope(ast::AST* scope_node);
    void analyzeControl(ast::AST* control_node);
    pkm::DataType analyzeObject(ast::AST* obj_node);
    pkm::DataType analyzeOperation(ast::AST* op_node);
    pkm::DataType analyzeFunction(ast::AST* func_node);
    pkm::DataType analyzeVariableDeclaration(ast::AST* var_decl_node);
    pkm::DataType analyzeVariable(ast::AST* var_node);

    Classes classes_;
    CompilationError error_;
    std::string class_name_;
    std::string method_name_;
    std::unordered_set<std::string> reserved_names_;
    std::unordered_map<std::string, pkm::DataType> locals_;
};

#endif // COMPILER_ANALYZER_ANALYZER_H