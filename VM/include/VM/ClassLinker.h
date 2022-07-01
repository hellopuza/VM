#ifndef VM_CLASSLINKER_H
#define VM_CLASSLINKER_H

#include "VM/Klass/KlassLoader.h"
#include "VM/Pkm/PkmClass.h"

class ClassLinker
{
public:
    ClassLinker() = default;
    void link(const Klasses& klasses);

    PkmClasses classes;

private:
    void appendClass(const std::string& klass);
    void getConstantPool(cp::ConstantPool* const_pool, const std::string& klass, size_t* pos);
    void getFields(PkmFields* fields, const std::string& klass, size_t* pos);
    void getMethods(PkmMethods* methods, pclass cls, const std::string& klass, size_t* pos);
    void linkClasses();

    cp::ConstantPool* const_pool_;
};

#endif // VM_CLASSLINKER_H