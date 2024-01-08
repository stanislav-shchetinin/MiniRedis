//
// Created by stass on 02.01.2024.
//

#ifndef MINIREDIS_NONBLOCK_H
#define MINIREDIS_NONBLOCK_H

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include "nonblock.h"
#include "network.h"
#include "request.h"

#define MAX_FD 200000
#define TIMEOUT (-1)
#define MAX_EVENTS 1

struct Conn {
    int fd;
    size_t rbuf_size;
    uint8_t rbuf[HEADER_SIZE + MAX_MSG];
    size_t wbuf_size;
    size_t wbuf_sent;
    uint8_t wbuf[HEADER_SIZE + MAX_MSG];
};

void fd_set_nb(int fd);
struct Conn* new_conn(int fd);
void epollin_part(struct Conn* conn);
void epollout_part(struct Conn* conn);
void add_new_fd(int epollfd, int fd, struct Conn** fd2conn);
bool epoll_wait_wrapper(int epollfd, struct epoll_event *evt);
uint8_t* read_text(struct Conn* conn);
void fill_buf(struct Conn* conn);

#endif //MINIREDIS_NONBLOCK_H
