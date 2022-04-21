#include "VM/Klass/KlassLoader.h"

#include <filesystem>
#include <fstream>
#include <sstream>

void KlassLoader::loadLib(const char* folder)
{
    for (const auto& entry : std::filesystem::directory_iterator(folder))
    {
        std::ifstream klass_file(entry.path().string());
        if (klass_file.is_open())
        {
            std::stringstream ss;
            ss << klass_file.rdbuf();
            klasses.emplace_back(ss.str());
        }
    }
}

int KlassLoader::loadUser(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++)
    {
        std::ifstream klass_file(argv[i]);
        if (klass_file.is_open())
        {
            std::stringstream ss;
            ss << klass_file.rdbuf();
            klasses.emplace_back(ss.str());
        }
        else
        {
            return i;
        }
    }

    return 0;
}