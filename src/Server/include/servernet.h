#ifndef MINIREDIS_SERVERNET_H
#define MINIREDIS_SERVERNET_H

#include <unistd.h>

int socket_initialization(int domain, int type, int protocol);
void bind_initialization(int socket, const struct sockaddr *address, socklen_t address_len);
void listen_initialization(int socket, int backlog);
int accept_initialization(int socket, struct sockaddr *address, socklen_t *address_len);
int epoll_initialization(int flags);

#endif //MINIREDIS_SERVERNET_H
