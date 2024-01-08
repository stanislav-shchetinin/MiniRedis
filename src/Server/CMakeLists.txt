cmake_minimum_required(VERSION 3.22)
project(Server C)

set(CMAKE_C_STANDARD 17)

include_directories(include)
include_directories(../../include)

add_executable(Server src/main.c
        ../general/network.c
        include/nonblock.h
        src/nonblock.c
        include/request.h
        src/request.c
        include/servernet.h
        src/servernet.c)
