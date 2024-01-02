//
// Created by stass on 13.12.2023.
//
#include "request.h"
#include "network.h"

int32_t request(int connfd){
    //+1 for '\0'
    char rbuf[HEADER_SIZE + MAX_MSG + 1];
    errno = 0;

    int32_t err = read_full(connfd, rbuf, HEADER_SIZE);

    if (err){
        if (errno == 0){
            printf("EOF\n");
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

    err = read_full(connfd, &rbuf[HEADER_SIZE], len);

    if (err){
        perror("read() error");
    }

    rbuf[4 + len] = '\0';
    printf("Client says: %s\n", &rbuf[HEADER_SIZE]);

    const char reply[] = "world";
    char wbuf[HEADER_SIZE + sizeof(reply)];
    len = (uint32_t) strlen(reply);

    memcpy(wbuf, &len, HEADER_SIZE);
    memcpy(&wbuf[4], reply, len);

    return write_all(connfd, wbuf, HEADER_SIZE + len);

}