//
// Created by stass on 08.01.2024.
//

#ifndef MINIREDIS_CLIENTNET_H
#define MINIREDIS_CLIENTNET_H

#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>

int socket_initialization(int domain, int type, int protocol);
void connect_initialization(int client_fd, const struct sockaddr *address, socklen_t address_len);

#endif //MINIREDIS_CLIENTNET_H
