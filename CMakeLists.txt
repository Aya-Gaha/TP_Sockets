cmake_minimum_required(VERSION 3.31)
project(TP_Sockets C)

set(CMAKE_C_STANDARD 99)

add_executable(TP_Sockets main.c)

target_link_libraries(TP_Sockets ws2_32)

