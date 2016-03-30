file(GLOB SRCS *.cpp)

add_executable(CPFA_loop_functions_test ${SRCS})

target_link_libraries(CPFA_loop_functions_test
        libgtest
        libgmock)

add_test(NAME CPFA_loop_functions_test
        COMMAND CPFA_loop_functions_test)