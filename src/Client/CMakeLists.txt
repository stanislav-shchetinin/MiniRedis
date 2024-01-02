cmake_minimum_required(VERSION 3.22)
project(Client C)

set(CMAKE_C_STANDARD 17)

include_directories(include)
include_directories(../../include)

add_executable(Client src/main.c
        src/query.c
        ../general/network.c)
