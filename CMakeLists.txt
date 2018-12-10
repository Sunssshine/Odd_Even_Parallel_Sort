cmake_minimum_required(VERSION 3.13)
project(Odd_Even_Parallel_Sort)


set(CMAKE_C_STANDARD 99)

add_executable(Odd_Even_Parallel_Sort main.c)
target_link_libraries(Odd_Even_Parallel_Sort pthread)
