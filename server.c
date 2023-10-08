//
// Created by stass on 11.09.2023.
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdbool.h>

#pragma comment(lib, "ws2_32.lib")

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
    int result = accept(socket, address, address_len);
    if (result == -1){
        perror("Invalid socket");
        exit(EXIT_FAILURE);
    }
    return result;
}

int main(){
    //ipv4, tcp
    int server_fd = socket_initialization(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(2343);
    bind_initialization(server_fd, (const struct sockaddr *) &address, sizeof(address));
    listen_initialization(server_fd, SOMAXCONN);

    while (true) {

        socklen_t address_len = sizeof(address);
        int client_fd = accept_initialization(server_fd, (struct sockaddr *)&address, &address_len);
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
        write(client_fd, buf, nread);

        close(client_fd);
    }

    close(server_fd);

    return 0;
}
