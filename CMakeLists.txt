cmake_minimum_required(VERSION 3.22.1)
project(MiniRedis C)

set(CMAKE_C_STANDARD 17)

include_directories(include)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        src/*.c
        src/*.h
        include/*.h
)

add_subdirectory(src/Server)
add_subdirectory(src/Client)


