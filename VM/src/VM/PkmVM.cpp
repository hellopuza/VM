#include "VM/PkmVM.h"

void PkmVM::destroyVM()
{
}

void PkmVM::loadClasses(PkmClasses* pclasses)
{
    classes_ = std::move(*pclasses);
}