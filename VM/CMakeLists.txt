cmake_minimum_required(VERSION 3.9)

file(GLOB_RECURSE VM_SOURCES *.cpp)
set(VM_SOURCES ${VM_SOURCES} PARENT_SCOPE)

set(VM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

function(add_exec EXEC_NAME)
    add_executable(${EXEC_NAME} EXCLUDE_FROM_ALL ${VM_SOURCES})
    target_include_directories(${EXEC_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    target_compile_options(${EXEC_NAME} PRIVATE -Wall -Werror -Wextra -Wpedantic -Wcast-qual -Wcast-align -Wconversion
        -Wsign-promo -Wfloat-equal -Wenum-compare -Wold-style-cast -Wredundant-decls -Wsign-conversion -Wnon-virtual-dtor
        -Wctor-dtor-privacy -Woverloaded-virtual -Wno-float-equal -pthread)

    set_target_properties(${EXEC_NAME} PROPERTIES
        CXX_STANDARD          20
        CXX_STANDARD_REQUIRED ON
    )

    if(ENABLE_ASAN)
        target_compile_options(${EXEC_NAME} PUBLIC -fsanitize=address -g)
        set_target_properties(${EXEC_NAME} PROPERTIES LINK_FLAGS "-fsanitize=address")
    endif()
endfunction()

list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_main.cpp)
list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/compiler_main.cpp)
set(VM_SOURCES ${VM_SOURCES} PARENT_SCOPE)

if(NOT ENABLE_TESTS AND NOT ENABLE_BENCHMARK)
    list(APPEND VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_main.cpp)
    add_exec(vm)

    list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_main.cpp)
    list(APPEND VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/compiler_main.cpp)
    add_exec(compiler)
endif()