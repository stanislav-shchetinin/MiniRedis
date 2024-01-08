//
// Created by stass on 08.01.2024.
//

#include "clientnet.h"

int socket_initialization(int domain, int type, int protocol){
    int result = socket(domain, type, protocol);
    if (result == -1){
        perror("Invalid socket");
        exit(EXIT_FAILURE);
    }
    return result;
}

void connect_initialization(int client_fd, const struct sockaddr *address, socklen_t address_len){
    int result = connect(client_fd, address, address_len);
    if (result == -1){
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }
}