#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include <string>
#include <vector>

class Compiler
{
public:
    Compiler() = default;
    bool load(const std::string& input_name);
    bool compile(const std::string& output_name);

private:
    std::vector<std::string> tokens_;
};

#endif // COMPILER_COMPILER_H