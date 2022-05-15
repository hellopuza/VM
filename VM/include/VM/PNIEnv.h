#ifndef VM_PNIENV_H
#define VM_PNIENV_H

#include "VM/ClassLinker.h"
#include "VM/PkmVM.h"

class PNIEnv
{
public:
    explicit PNIEnv(PkmVM* pvm);

    void loadClasses(PkmClasses* pclasses);

    pclass findClass(const std::string& class_name);
    static pmethodID getMethodID(pclass cls, const std::string& met_name);
    void callMethod(pclass cls, pmethodID mid);

private:
    PkmVM* pvm_;
    PkmClasses classes_;
};

#endif // VM_PNIENV_H