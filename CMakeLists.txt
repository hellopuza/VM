cmake_minimum_required(VERSION 3.16)
project(VM VERSION 1.0.0 DESCRIPTION "Virtual machine for running bytecode")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ALL_CXX_SOURCE_FILES *.cpp *.h)

find_program(CLANG_FORMAT "clang-format")
add_custom_target(
    format
    COMMAND clang-format-12
    -i
    -style=file
    ${ALL_CXX_SOURCE_FILES}
)

if (ADD_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY
        clang-tidy;
        --format-style=file;
        --header-filter=*.h;
        --line-filter=[{"name":"*.c","lines":[[9999,9999]]},{"name":".h"},{"name":".cpp"}]
    )
endif()

add_subdirectory(VM)

if (ENABLE_TESTS)
    add_subdirectory(test)
endif()

if (ENABLE_BENCHMARK)
    add_subdirectory(benchmark)
endif()

set(RESOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/pkm)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/VM)