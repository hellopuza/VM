#ifndef VM_PKMVM_H
#define VM_PKMVM_H

<<<<<<< HEAD
#include "VM/PkmVMInitArgs.h"
#include "VM/Interpreter.h"

#include <string>
=======
#include "VM/ClassLinker.h"

#include <unordered_map>
>>>>>>> main

class PkmVM
{
public:
<<<<<<< HEAD
    explicit PkmVM(const PkmVMInitArgs args);
    void destroyVM();

private:
    PkmVMInitArgs args_;
    Interpreter interpreter_;
=======
    PkmVM() = default;
    static void destroyVM();

    void loadClasses(PkmClasses* pclasses);

private:
    PkmClasses classes_;
>>>>>>> main
};

#endif // VM_PNI_H