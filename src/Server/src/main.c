#define _GNU_SOURCE

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include "network.h"
#include "nonblock.h"
#include "servernet.h"

#pragma comment(lib, "ws2_32.general")

int server_fd;

void close_all(void){
    close(server_fd);
}

int main(){
    //ipv4, tcp
    atexit(close_all);
    server_fd = socket_initialization(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    bind_initialization(server_fd, (const struct sockaddr *) &address, sizeof(address));
    listen_initialization(server_fd, SOMAXCONN);
    socklen_t address_len = sizeof(address);

    fd_set_nb(server_fd);

    //epoll
    int epollfd = epoll_initialization(0);
    struct epoll_event evt = {.events = EPOLLIN | EPOLLOUT | EPOLLET, .data.fd = server_fd};
    epoll_ctl(epollfd, EPOLL_CTL_ADD, server_fd, &evt);

    struct Conn* fd2conn[MAX_FD];

    while (true){

        evt.events = 0;
        errno = 0;
        if (!epoll_wait_wrapper(epollfd, &evt)) continue;

        if (evt.data.fd == server_fd){
            int client_fd = accept_initialization(server_fd,
                                                  (struct sockaddr *)&address,
                                                          &address_len);
            add_new_fd(epollfd, client_fd, fd2conn);
        } else {
            struct Conn* conn = fd2conn[evt.data.fd];
            if ((evt.events & (EPOLLIN | EPOLLOUT)) == (EPOLLIN | EPOLLOUT)) {
                epollin_part(conn);
                epollout_part(conn);
            } else if ((evt.events & EPOLLIN) == EPOLLIN){
                epollin_part(conn);
            } else if ((evt.events & EPOLLOUT) == EPOLLOUT) {
                epollout_part(conn);
            }
        }

    }

    return 0;
}