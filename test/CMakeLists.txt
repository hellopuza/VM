cmake_minimum_required(VERSION 3.16)

include(FetchContent)
FetchContent_Declare(
  gtest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

enable_testing()

file(GLOB_RECURSE VM_TEST_SOURCES *.cpp)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

flex_target(lexer
  ${CMAKE_CURRENT_SOURCE_DIR}/../VM/include/Compiler/Lexer/lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc
)

bison_target(parser
  ${CMAKE_CURRENT_SOURCE_DIR}/../VM/include/Compiler/Parser/parser.y
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/parser.hh"
)

add_flex_bison_dependency(lexer parser)

file(GLOB_RECURSE VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../VM/*.cpp)
list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../VM/src/disassembler_main.cpp)
list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../VM/src/compiler_main.cpp)
list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../VM/src/vm_main.cpp)

list(APPEND VM_SOURCES ${BISON_parser_OUTPUTS})
list(APPEND VM_SOURCES ${FLEX_lexer_OUTPUTS})

set(EXEC_NAME vm_test)

add_executable(${EXEC_NAME} ${VM_TEST_SOURCES} ${VM_SOURCES})
target_include_directories(${EXEC_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../VM/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../VM/include/Common
    PUBLIC ${CMAKE_SOURCE_DIR}/lib
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_options(${EXEC_NAME} PRIVATE -Wall -Werror -Wextra -Wpedantic -Wcast-qual -Wcast-align
        -Wsign-promo -Wfloat-equal -Wenum-compare -Wredundant-decls -Wnon-virtual-dtor
        -Wctor-dtor-privacy -Woverloaded-virtual -Wno-float-equal -pthread -std=c++2a
)

add_custom_target(run_${EXEC_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME})
add_dependencies(run_${EXEC_NAME} ${EXEC_NAME})

if(ENABLE_ASAN)
    target_compile_options(${EXEC_NAME} PUBLIC -fsanitize=address -g)
    set_target_properties(${EXEC_NAME} PROPERTIES LINK_FLAGS "-fsanitize=address")
endif()

add_custom_target(run_valgrind_${EXEC_NAME} COMMAND valgrind ./${EXEC_NAME})
add_dependencies(run_valgrind_${EXEC_NAME} ${EXEC_NAME})

target_link_libraries(${EXEC_NAME} gtest_main)

include(GoogleTest)
gtest_discover_tests(${EXEC_NAME} PROPERTIES LABELS "unit")

list(REMOVE_ITEM VM_SOURCES ${BISON_parser_OUTPUTS})
list(REMOVE_ITEM VM_SOURCES ${FLEX_lexer_OUTPUTS})