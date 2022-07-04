# VM (Virtual machine)

This project is essentially a programming language. It includes a compiler to get the bytecode, and the virtual machine itself for running bytecode.

## Technical description

### Installation

First you need clone the project and make build directory.

There are many build targets in the project, so if you want to use all of them, you need to check if you have the following programs installed:

* [graphviz](https://graphviz.org/download/)

* [valgrind](https://valgrind.org/)

* [google benchmark](https://github.com/google/benchmark)

After that you can build a project:
```
cd build
cmake ..
```

The following flags are available for building a project:

* ADD_CLANG_TIDY
* ENABLE_TESTS
* ENABLE_BENCHMARK
* ENABLE_ASAN

So you can build this project like this:
```
cmake .. -DENABLE_TESTS=ON -DENABLE_ASAN=ON
make run_vm_test
```

Main targets to run:
* vm
* compiler
* run_vm_test

### Using
To build this project do
```
make -j compiler
make -j vm
```

Write program with .pkm file extension and run compiler to build class files
```
./VM/compiler location/to/program1.pkm ...
```

Then run vm with all required class files
```
./VM/vm Main.klass MyClass.klass ...
```
