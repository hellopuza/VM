cmake_minimum_required(VERSION 3.16.3)

set(CMAKE_CXX_STANDARD 20)

find_package(benchmark REQUIRED)

file(GLOB_RECURSE VM_BENCHMARK_SOURCES *.cpp)

set(EXEC_NAME vm_benchmark)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")

add_executable(${EXEC_NAME} ${VM_BENCHMARK_SOURCES} ${VM_SOURCES})
target_include_directories(${EXEC_NAME} PUBLIC ${VM_INCLUDE})

add_custom_target(run_${EXEC_NAME}
                COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME})
add_dependencies(run_${EXEC_NAME} ${EXEC_NAME})

target_link_libraries(${EXEC_NAME} benchmark::benchmark)