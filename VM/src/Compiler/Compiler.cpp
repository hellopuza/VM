#include "Compiler/Compiler.h"

#include <fstream>

bool Compiler::load(const std::string& input_name)
{
    std::ifstream file(input_name);
    if (file.is_open())
    {
        tokens_.clear();
        std::string token;
        while (!file.eof())
        {
            file >> token;
            tokens_.push_back(token);
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool Compiler::compile(const std::string& output_name)
{
    std::ofstream file(output_name, std::ofstream::out);
    if (file.is_open())
    {
        for (const auto& token : tokens_)
        {
            file << token;
        }
    }
    else
    {
        return false;
    }

    return true;
}