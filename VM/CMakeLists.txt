cmake_minimum_required(VERSION 3.9)

set(EXEC_NAME VM)

file(GLOB_RECURSE VM_SOURCES *.cpp)
set(VM_SOURCES ${VM_SOURCES} PARENT_SCOPE)

set(VM_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wpedantic -Wcast-qual -Wcast-align -Wconversion \
    -Wsign-promo -Wfloat-equal -Wenum-compare -Wold-style-cast -Wredundant-decls -Wsign-conversion -Wnon-virtual-dtor \
    -Wctor-dtor-privacy -Woverloaded-virtual -Wno-float-equal -O3 -pthread"
)

if(ADD_TESTING)
    list(REMOVE_ITEM VM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
    set(VM_SOURCES ${VM_SOURCES} PARENT_SCOPE)
else()
    add_executable(${EXEC_NAME} ${VM_SOURCES})
    target_include_directories(${EXEC_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    set_target_properties(${EXEC_NAME} PROPERTIES
        CXX_STANDARD          20
        CXX_STANDARD_REQUIRED ON
    )

    if(ADD_SANITIZERS)
        target_compile_options(${EXEC_NAME} PUBLIC -fsanitize=address -g)
        set_target_properties(${EXEC_NAME} PROPERTIES LINK_FLAGS "-fsanitize=address")
    endif()

    add_custom_target(run_${EXEC_NAME}
                        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME})
    add_dependencies(run_${EXEC_NAME} ${EXEC_NAME})

    add_custom_target(run_valgrind_${EXEC_NAME}
                        COMMAND valgrind ./${EXEC_NAME})
    add_dependencies(run_valgrind_${EXEC_NAME} ${EXEC_NAME})
endif()