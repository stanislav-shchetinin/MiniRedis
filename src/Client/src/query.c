//
// Created by stass on 02.01.2024.
//

#include "query.h"
#include "network.h"

int32_t query(int fd, const char *text) {
    int32_t err = send_msg(fd, text);

    if (err) {
        perror("Send message error");
        return err;
    }

    char rbuf[HEADER_SIZE + MAX_MSG + 1];
    err = read_msg(fd, rbuf);

    if (err == -1){
        perror("Read message error");
        return err;
    }

    printf("%s\n", &rbuf[HEADER_SIZE]);
    return 0;
}