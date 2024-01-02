//
// Created by stass on 02.01.2024.
//

#include "query.h"
#include "network.h"

int32_t query(int fd, const char *text) {
    uint32_t len = (uint32_t)strlen(text);
    if (len > MAX_MSG) {
        fprintf(stderr, "Too long\n");
        return -1;
    }

    char wbuf[HEADER_SIZE + len + 1];
    memcpy(wbuf, &len, HEADER_SIZE);  // assume little endian
    strcpy(&wbuf[HEADER_SIZE], text);

    int32_t err = write_all(fd, wbuf, HEADER_SIZE + len);
    if (err) {
        return err;
    }

    // 4 bytes header
    char rbuf[HEADER_SIZE + MAX_MSG + 1];
    errno = 0;
    err = read_full(fd, rbuf, HEADER_SIZE);
    if (err) {
        if (errno == 0) {
            printf("EOF\n");
        } else {
            perror("read() error");
        }
        return err;
    }

    memcpy(&len, rbuf, HEADER_SIZE);  // assume little endian
    if (len > MAX_MSG) {
        fprintf(stderr, "Too long");
        return -1;
    }

    err = read_full(fd, &rbuf[HEADER_SIZE], len);
    if (err) {
        perror("read() error");
        return err;
    }

    rbuf[HEADER_SIZE + len] = '\0';
    printf("Server says: %s\n", &rbuf[HEADER_SIZE]);
    return 0;
}