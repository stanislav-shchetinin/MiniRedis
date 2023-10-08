//
// Created by stass on 08.10.2023.
//

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

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

int main(){

    int client_fd = socket_initialization(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(2343);
    connect_initialization(client_fd, (struct sockaddr *) &address, sizeof(address));

    write(client_fd, "Hello\n", 6);

    char buf[256];
    ssize_t nread = read(client_fd, buf, 256);

    if (nread == -1){
        perror("Read failed");
        exit(EXIT_FAILURE);
    }
    if (nread == 0){
        printf("End of file occurred\n");
    }

    write(STDOUT_FILENO, buf, nread);
    close(client_fd);

    return 0;
}