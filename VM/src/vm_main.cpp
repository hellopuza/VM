#include "VM/ClassLinker.h"
#include "VM/Klass/KlassLoader.h"
#include "VM/PNI.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#define CHECK_ERROR(cond, message)      \
    if (cond) {                         \
        std::cout << (message) << "\n"; \
        return -1;                      \
    } //

const char* const BIN_FOLDER = "./pkm/bin";

int main(int argc, char* argv[])
{
    KlassLoader kl;
    kl.loadLib(BIN_FOLDER);
    int err = kl.loadUser(argc, argv);
    CHECK_ERROR(err, "Klass file not loaded: " + std::string(argv[err]));

    ClassLinker cl;
    cl.link(kl.klasses);

    PkmVM* pvm = nullptr;
    PNIEnv* env = nullptr;

    PNI_createVM(&pvm, &env);
    env->loadClasses(&cl.classes);

    pclass cls = env->findClass("Main");
    CHECK_ERROR(cls == nullptr, "Class Main not found");

    pmethodID mid = PNIEnv::getMethodID(cls, "main");
    CHECK_ERROR(cls == nullptr, "Method main not found");

    env->callMethod(cls, mid);

    PkmVM::destroyVM();
    delete pvm;
    delete env;

    return 0;
}