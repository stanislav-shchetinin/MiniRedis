#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "servernet.h"

int socket_initialization(int domain, int type, int protocol){
    int result = socket(domain, type, protocol);
    if (result == -1){
        perror("Invalid socket");
        exit(EXIT_FAILURE);
    }
    return result;
}

void bind_initialization(int socket, const struct sockaddr *address, socklen_t address_len){
    int result = bind(socket, address, address_len);
    if (result == -1){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

void listen_initialization(int socket, int backlog){
    int result = listen(socket, backlog);
    if (result == -1){
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
}

int accept_initialization(int socket, struct sockaddr *address, socklen_t *address_len){
    int result = accept4(socket, address, address_len, SOCK_NONBLOCK);
    if (result == -1){
        perror("Invalid socket");
        exit(EXIT_FAILURE);
    }
    return result;
}

int epoll_initialization(int flags){
    int epollfd = epoll_create1(flags);
    if (epollfd < 0){
        perror("epoll");
        exit(EXIT_FAILURE);
    }
    return epollfd;
}