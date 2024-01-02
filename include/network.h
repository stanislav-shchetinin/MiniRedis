//
// Created by stass on 02.01.2024.
//

#ifndef MINIREDIS_NETWORK_H
#define MINIREDIS_NETWORK_H

#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#define MAX_MSG 4096
#define HEADER_SIZE 4

int32_t read_full(int fd, char* buf, size_t n);
int32_t write_all(int fd, char* buf, size_t n);

#endif //MINIREDIS_NETWORK_H
