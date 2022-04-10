cmake_minimum_required(VERSION 3.9)
project(VM VERSION 1.0.0 DESCRIPTION "Virtual machine for running bytecode")

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