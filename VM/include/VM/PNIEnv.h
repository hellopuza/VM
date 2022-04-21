#ifndef VM_PNIENV_H
#define VM_PNIENV_H

#include "VM/PkmVM.h"

using pclass = PkmClass*;
using pmethodID = PkmMethod*;

class PNIEnv
{
public:
    explicit PNIEnv(PkmVM* pvm);

    void loadClasses(PkmClasses* pclasses);

    pclass findClass(const std::string& class_name);
    static pmethodID getMethodID(pclass cls, const std::string& met_name);
    static void callMethod(pclass cls, pmethodID mid);

    PkmVM* pvm_;
private:
    PkmClasses classes_;
};

#endif // VM_PNIENV_H