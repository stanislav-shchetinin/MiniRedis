//
// Created by stass on 11.09.2023.
//
#include <winsock2.h>
#include <stdint.h>
#include <ws2tcpip.h>

uint64_t SocketInitialization(int32_t domain, int32_t type, int32_t protocol){
    uint64_t result = socket(domain, type, protocol);
    if (result == INVALID_SOCKET){
        perror("Invalid socket");
        exit(EXIT_FAILURE);
    }
    return result;
}

void BindInitialization(uint64_t socket, const struct sockaddr *address, socklen_t address_len){
    int32_t result = bind(socket, address, address_len);
    if (result == SOCKET_ERROR){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

void ListenInitialization(uint64_t socket, ){

}

int main(){
    //ipv4, tcp
    uint64_t server_fd = SocketInitialization(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address = {0};
    address.sin_addr = AF_INET;
    address.sin_port = htons(34543);
    BindInitialization(server_fd, &address, sizeof(address));
    return 0;
}
