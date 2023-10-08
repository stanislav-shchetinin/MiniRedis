cmake_minimum_required(VERSION 3.26)
project(MiniRedis C)

set(CMAKE_C_STANDARD 17)

add_executable(MiniRedis main.c
        server.c
)
