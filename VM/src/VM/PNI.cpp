#include "VM/PNI.h"

void PNI_createVM(PkmVM** pvm, PNIEnv** env, PkmVMInitArgs* init_args)
{
    *pvm = new PkmVM(*init_args);
    *env = new PNIEnv;
}