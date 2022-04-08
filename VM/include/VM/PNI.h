#ifndef VM_PNI_H
#define VM_PNI_H

#include "VM/PkmVM.h"
#include "VM/PkmVMInitArgs.h"
#include "VM/PNIEnv.h"

void PNI_createVM(PkmVM** pvm, PNIEnv** env, PkmVMInitArgs* init_args);

#endif // VM_PNI_H