//
// Created by stass on 08.10.2023.
//
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <stdbool.h>
#include "query.h"
#include "network.h"

static int socket_initialization(int domain, int type, int protocol){
    int result = socket(domain, type, protocol);
    if (result == -1){
        perror("Invalid socket");
        exit(EXIT_FAILURE);
    }
    return result;
}

static void connect_initialization(int client_fd, const struct sockaddr *address, socklen_t address_len){
    int result = connect(client_fd, address, address_len);
    if (result == -1){
        perror("Connect failed");
        exit(EXIT_FAILURE);
    }
}

int main(){

    int client_fd = socket_initialization(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(2343);
    connect_initialization(client_fd, (struct sockaddr *) &address, sizeof(address));

    while (true){
        char text[MAX_MSG] = {0};
        scanf("%s", text);
        printf("Text: %s\n", text);
        int32_t err = query(client_fd, text);
        if (err) break;
    }

    return 0;
}