//
// Created by stass on 02.01.2024.
//

#include "network.h"

#define IO_ALL(type, fd, buf, n)                \
    while (n > 0){                              \
        ssize_t rv = type(fd, buf, n);          \
        if (rv <= 0) {                          \
            return -1;                          \
        }                                       \
        assert((size_t)rv <= n);                \
        n -= (size_t) rv;                       \
        buf += rv;                              \
    }                                           \
    return 0;


int32_t read_full(int fd, char* buf, size_t n) {
    IO_ALL(read, fd, buf, n);
}

int32_t write_all(int fd, char* buf, size_t n) {
    IO_ALL(write, fd, buf, n);
}