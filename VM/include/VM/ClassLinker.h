#ifndef VM_CLASSLINKER_H
#define VM_CLASSLINKER_H

#include "VM/Klass/KlassLoader.h"
#include "VM/Pkm/PkmClass.h"

using PkmClasses = std::unordered_map<std::string, PkmClass>;

class ClassLinker
{
public:
    ClassLinker() = default;
    void link(Klasses* klasses);

    PkmClasses classes;

private:
    void appendClass(std::string* klass);
    static std::string getString(const std::string& klass, size_t* pos);
    void getConstantPool(ConstantPool* const_pool, const std::string& klass, size_t* pos);
    void getFields(PkmFields* fields, const std::string& klass, size_t* pos);
    void getMethods(PkmMethods* methods, const std::string& klass, size_t* pos);

    std::vector<std::unique_ptr<AbstractType>> const_pool_;
};

#endif // VM_CLASSLINKER_H