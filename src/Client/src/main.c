//
// Created by stass on 08.10.2023.
//
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <stdbool.h>
#include "query.h"
#include "network.h"
#include "clientnet.h"

int main(){

    int client_fd = socket_initialization(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
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