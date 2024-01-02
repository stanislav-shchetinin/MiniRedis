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

int32_t send_msg(int fd, const char* text){
    uint32_t len = (uint32_t)strlen(text);
    if (len > MAX_MSG) {
        fprintf(stderr, "Too long\n");
        return -1;
    }

    char wbuf[HEADER_SIZE + len + 1];
    memcpy(wbuf, &len, HEADER_SIZE);  // assume little endian
    strcpy(&wbuf[HEADER_SIZE], text);

    return write_all(fd, wbuf, HEADER_SIZE + len);
}

int32_t read_msg(int fd, char* rbuf) {
    //+1 for '\0'
    errno = 0;

    int32_t err = read_full(fd, rbuf, HEADER_SIZE);

    if (err){
        if (errno == 0){
            printf("EOF\n");
            return EOF_CODE;
        } else {
            perror("read() error");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, HEADER_SIZE);

    if (len > MAX_MSG){
        fprintf(stderr, "Too long");
        return -1;
    }

    err = read_full(fd, &rbuf[HEADER_SIZE], len);

    if (err){
        perror("read() error");
    }

    rbuf[4 + len] = '\0';
    return 0;
}