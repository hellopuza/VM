cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(gtest QUIET URL https://github.com/google/googletest/archive/release-1.10.0.tar.gz)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)

enable_testing()

file(GLOB_RECURSE VM_TEST_SOURCES *.cpp)

set(EXEC_NAME vm_test)

add_executable(${EXEC_NAME} ${VM_TEST_SOURCES} ${VM_SOURCES})
target_include_directories(${EXEC_NAME} PUBLIC ${VM_INCLUDE})

set_target_properties(${EXEC_NAME} PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED ON
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