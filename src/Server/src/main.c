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

int main(){
    //ipv4, tcp
    int server_fd = socket_initialization(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(2343);
    bind_initialization(server_fd, (const struct sockaddr *) &address, sizeof(address));
    listen_initialization(server_fd, SOMAXCONN);
    socklen_t address_len = sizeof(address);

    //epoll
    int epollfd = epoll_initialization(0);
    struct epoll_event evt = {.events = EPOLLIN, .data.fd = server_fd};
    epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &evt);

    while (true) {

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
            struct epoll_event evt = {.events = EPOLLIN | EPOLLET, .data.fd = client_fd};
            epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &evt);
        } else {
            int32_t err = request(evt.data.fd);
            if (err){
                close(evt.data.fd);
                break;
            }
        }

    }

    close(server_fd);

    return 0;
}
