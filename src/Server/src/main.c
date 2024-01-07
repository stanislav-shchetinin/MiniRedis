//
// Created by stass on 11.09.2023.
//

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include "request.h"
#include "network.h"
#include "nonblock.h"

#pragma comment(lib, "ws2_32.general")

static int socket_initialization(int domain, int type, int protocol){
    int result = socket(domain, type, protocol);
    if (result == -1){
        perror("Invalid socket");
        exit(EXIT_FAILURE);
    }
    return result;
}

static void bind_initialization(int socket, const struct sockaddr *address, socklen_t address_len){
    int result = bind(socket, address, address_len);
    if (result == -1){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

static void listen_initialization(int socket, int backlog){
    int result = listen(socket, backlog);
    if (result == -1){
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

static int accept_initialization(int socket, struct sockaddr *address, socklen_t *address_len){
    int result = accept4(socket, address, address_len, SOCK_NONBLOCK);
    if (result == -1){
        perror("Invalid socket");
        exit(EXIT_FAILURE);
    }
    return result;
}

static int epoll_initialization(int flags){
    int epollfd = epoll_create1(flags);
    if (epollfd < 0){
        perror("epoll");
        exit(EXIT_FAILURE);
    }
    return epollfd;
}

#define MAX_FD 200000

struct Conn {
    int fd;
    size_t rbuf_size;
    uint8_t rbuf[4 + MAX_MSG];
    size_t wbuf_size;
    size_t wbuf_sent;
    uint8_t wbuf[4 + MAX_MSG];
};

struct Conn* new_conn(int fd){
    struct Conn* conn = malloc(sizeof(struct Conn));
    if (conn){
        conn->fd = fd;
        conn->rbuf_size = 0;
        conn->wbuf_size = 0;
        conn->wbuf_sent = 0;
    } else {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

int main(){
    //ipv4, tcp
    int server_fd = socket_initialization(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(2343);
    bind_initialization(server_fd, (const struct sockaddr *) &address, sizeof(address));
    listen_initialization(server_fd, SOMAXCONN);
    socklen_t address_len = sizeof(address);

    fd_set_nb(server_fd);

    //epoll
    int epollfd = epoll_initialization(0);
    struct epoll_event evt = {.events = EPOLLIN | EPOLLET, .data.fd = server_fd};
    epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &evt);

    struct Conn* fd2conn[MAX_FD];

    while (true){
        evt.events = 0;

        int timeout = -1;
        errno = 0;

        if (epoll_wait(epollfd, &evt, 1, timeout) < 1){
            if (errno == EINTR){
                continue;
            }
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (evt.data.fd == server_fd){

            int client_fd = accept_initialization(server_fd,
                                                  (struct sockaddr *)&address,
                                                          &address_len);
            fd2conn[client_fd] = new_conn(client_fd);
            struct epoll_event evt_new = {.events = EPOLLIN | EPOLLOUT | EPOLLET, .data.fd = client_fd};
            epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &evt_new);

        } else {
            if ((evt.events & (EPOLLIN | EPOLLOUT)) == (EPOLLIN | EPOLLOUT)) {
                printf("EPOLLIN | EPOLLOUT");

                int len;
                struct Conn* conn = fd2conn[evt.data.fd];
                read(evt.data.fd, &len, HEADER_SIZE);
                read(evt.data.fd, conn->rbuf, len);
                conn->rbuf_size = HEADER_SIZE + len;

                conn->rbuf[conn->rbuf_size] = '\0';
                printf("Client says: %s", &conn->rbuf[HEADER_SIZE]);

                memcpy(conn->wbuf, &len, HEADER_SIZE);
                memcpy(&conn->wbuf[HEADER_SIZE], conn->rbuf, len);
                conn->wbuf_size = HEADER_SIZE + len;

                conn = fd2conn[evt.data.fd];
                write(evt.data.fd, conn->wbuf, conn->wbuf_size);

                conn->wbuf_size = 0;

            } else if ((evt.events & EPOLLIN) == EPOLLIN){
                printf("EPOLLIN");
                int len;
                struct Conn* conn = fd2conn[evt.data.fd];
                read(evt.data.fd, &len, HEADER_SIZE);
                read(evt.data.fd, conn->rbuf, len);
                conn->rbuf_size = HEADER_SIZE + len;

                conn->rbuf[conn->rbuf_size] = '\0';
                printf("Client says: %s", &conn->rbuf[HEADER_SIZE]);

                memcpy(conn->wbuf, &len, HEADER_SIZE);
                memcpy(&conn->wbuf[HEADER_SIZE], conn->rbuf, len);
                conn->wbuf_size = HEADER_SIZE + len;

            } else if ((evt.events & EPOLLOUT) == EPOLLOUT) {
                printf("EPOLLOUT");
                struct Conn* conn = fd2conn[evt.data.fd];
                write(evt.data.fd, conn->wbuf, conn->wbuf_size);

                conn->wbuf_size = 0;

            }
        }

    }

    close(server_fd);

    return 0;
}
