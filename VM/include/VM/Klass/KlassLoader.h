#ifndef VM_KLASS_KLASSLOADER_H
#define VM_KLASS_KLASSLOADER_H

#include <string>
#include <vector>

using Klasses = std::vector<std::string>;

class KlassLoader
{
public:
    KlassLoader() = default;
    void loadLib(const char* folder);
    int loadUser(int argc, char* argv[]);

    Klasses klasses;
};

#endif // VM_KLASS_KLASSLOADER_H