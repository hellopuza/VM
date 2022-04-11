cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 20)

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
list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../VM/src/compiler_main.cpp)
list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../VM/src/vm_main.cpp)

list(APPEND VM_SOURCES ${BISON_parser_OUTPUTS})
list(APPEND VM_SOURCES ${FLEX_lexer_OUTPUTS})

find_package(benchmark REQUIRED)

file(GLOB_RECURSE VM_BENCHMARK_SOURCES *.cpp)

set(EXEC_NAME vm_benchmark)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")

add_executable(${EXEC_NAME} ${VM_BENCHMARK_SOURCES} ${VM_SOURCES})
target_include_directories(${EXEC_NAME}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../VM/include
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../VM/include/Common
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_target(run_${EXEC_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME})
add_dependencies(run_${EXEC_NAME} ${EXEC_NAME})

target_link_libraries(${EXEC_NAME} benchmark::benchmark)

list(REMOVE_ITEM VM_SOURCES ${BISON_parser_OUTPUTS})
list(REMOVE_ITEM VM_SOURCES ${FLEX_lexer_OUTPUTS})