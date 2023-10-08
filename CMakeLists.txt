cmake_minimum_required(VERSION 3.22.1)
project(MiniRedis C)

set(CMAKE_C_STANDARD 17)

add_executable(MiniRedis
        server.c
        client.c
)
