cmake_minimum_required(VERSION 3.16)

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

file(GLOB_RECURSE COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/Common/*.cpp)

function(add_exec EXEC_NAME SOURCES)
    add_executable(${EXEC_NAME} EXCLUDE_FROM_ALL ${${SOURCES}})
    target_include_directories(${EXEC_NAME}
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/Common
        PUBLIC ${CMAKE_SOURCE_DIR}/lib
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_compile_options(${EXEC_NAME} PRIVATE -Wall -Werror -Wextra -Wpedantic -Wcast-qual -Wcast-align
        -Wsign-promo -Wfloat-equal -Wenum-compare -Wredundant-decls -Wnon-virtual-dtor
        -Wctor-dtor-privacy -Woverloaded-virtual -Wno-float-equal -pthread -std=c++2a)

    if(ENABLE_ASAN)
        target_compile_options(${EXEC_NAME} PUBLIC -fsanitize=address -g)
        set_target_properties(${EXEC_NAME} PROPERTIES LINK_FLAGS "-fsanitize=address")
    endif()
endfunction()

file(GLOB_RECURSE DIS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/Disassembler/*.cpp)
list(APPEND DIS_SOURCES ${COMMON_SOURCES})
list(APPEND DIS_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/disassembler_main.cpp)
add_exec(dis DIS_SOURCES)

file(GLOB_RECURSE COMPILER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/Compiler/*.cpp)
list(APPEND COMPILER_SOURCES ${COMMON_SOURCES})
list(APPEND COMPILER_SOURCES ${BISON_parser_OUTPUTS})
list(APPEND COMPILER_SOURCES ${FLEX_lexer_OUTPUTS})
list(APPEND COMPILER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/compiler_main.cpp)
add_exec(compiler COMPILER_SOURCES)

file(GLOB_RECURSE VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/VM/*.cpp)
list(APPEND VM_SOURCES ${COMMON_SOURCES})
list(APPEND VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/vm_main.cpp)
add_exec(vm VM_SOURCES)