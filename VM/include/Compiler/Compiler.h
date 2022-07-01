#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "Compiler/Analyzer/Analyzer.h"
#include "Compiler/AST/AST.h"

class Compiler
{
public:
    Compiler() = default;
    Compiler(const char* lib_folder);
    void load(const std::string& input_name);
    void compile(const std::string& code_ext);
    void printErrors(std::ostream& os);
    CompilationError getError() const;

private:
    void loadFile(const std::string& input_name, std::ifstream* file);
    const char* getErrorString() const;

    Analyzer anal_;
    std::vector<std::pair<std::string, ast::AST>> classes_;

    CompilationError error_;
    std::string current_filename_;
    std::unordered_map<std::string, std::vector<std::string>> programs_;
};

#endif // COMPILER_COMPILER_H