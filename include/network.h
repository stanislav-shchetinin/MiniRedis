//
// Created by stass on 02.01.2024.
//

#ifndef MINIREDIS_NETWORK_H
#define MINIREDIS_NETWORK_H

#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define MAX_MSG 4096
#define HEADER_SIZE 4
#define EOF_CODE 2

int32_t read_full(int fd, char* buf, size_t n);
int32_t write_all(int fd, char* buf, size_t n);
int32_t send_msg(int fd, const char* text);
int32_t read_msg(int fd, char* rbuf);

#endif //MINIREDIS_NETWORK_H
