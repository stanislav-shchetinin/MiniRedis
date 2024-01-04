//
// Created by stass on 13.12.2023.
//
#include "request.h"
#include "network.h"

int32_t request(int connfd){
    char rbuf[HEADER_SIZE + MAX_MSG + 1];

    int32_t err = read_msg(connfd, rbuf);

    if (err == -1){
        perror("Read message error");
        return err;
    }
    if (err == EOF_CODE) return -1;

    printf("Client says: %s\n", &rbuf[HEADER_SIZE]);

    err = send_msg(connfd, &rbuf[HEADER_SIZE]);

    if (err) {
        perror("Send message error");
        return err;
    }

    return 0;
}