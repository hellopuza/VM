#include "VM/PNI.h"

void PNI_createVM(PkmVM** pvm, PNIEnv** env)
{
    *pvm = new PkmVM;
    *env = new PNIEnv(*pvm);
}