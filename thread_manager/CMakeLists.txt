cmake_minimum_required(VERSION 3.14)
project(VM VERSION 1.0.0 DESCRIPTION "Virtual machine for running bytecode")

set(CMAKE_CXX_STANDARD 20)

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

add_executable(thread_manager_test ./tests/test.cpp ./src/Manager.cpp)

target_link_libraries(thread_manager_test gtest_main)

include(GoogleTest)
gtest_discover_tests(thread_manager_test)