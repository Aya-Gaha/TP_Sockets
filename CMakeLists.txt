cmake_minimum_required(VERSION 3.31)
project(TP_Sockets C)

set(CMAKE_C_STANDARD 99)

add_executable(TP_Sockets main.c)

target_link_libraries(TP_Sockets ws2_32)

# Task 3: TCP Client/Server for Time Transfer
add_executable(task3_server task3_tcp/tcp_server.c)
add_executable(task3_client task3_tcp/tcp_client.c)