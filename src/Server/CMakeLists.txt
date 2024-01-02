cmake_minimum_required(VERSION 3.22)
project(Server C)

set(CMAKE_C_STANDARD 17)

include_directories(include)
include_directories(../../include)

add_executable(Server src/main.c
        src/request.c
        ../general/network.c)
