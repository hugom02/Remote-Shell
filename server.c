#include "../headers/server.h"

int createSocket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int bindSocket(int socketFD, char* port, char* ip) {
    // Création d'un socket
    int rv;
    struct addrinfo hints, *res, *binding;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


