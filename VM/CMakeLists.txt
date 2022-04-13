cmake_minimum_required(VERSION 3.9)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

flex_target(lexer
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Compiler/Lexer/lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc
)

bison_target(parser
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Compiler/Parser/parser.y
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/parser.hh"
)

add_flex_bison_dependency(lexer parser)

file(GLOB_RECURSE VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

list(APPEND VM_SOURCES ${BISON_parser_OUTPUTS})
list(APPEND VM_SOURCES ${FLEX_lexer_OUTPUTS})

function(add_exec EXEC_NAME)
    add_executable(${EXEC_NAME} EXCLUDE_FROM_ALL ${VM_SOURCES})
    target_include_directories(${EXEC_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/Common
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_compile_options(${EXEC_NAME} PRIVATE -Wall -Werror -Wextra -Wpedantic -Wcast-qual -Wcast-align -Wconversion
        -Wsign-promo -Wfloat-equal -Wenum-compare -Wredundant-decls -Wsign-conversion -Wnon-virtual-dtor
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

list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/compiler_main.cpp)
list(APPEND VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_main.cpp)
add_exec(vm)

list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_main.cpp)
list(APPEND VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/compiler_main.cpp)
add_exec(compiler)