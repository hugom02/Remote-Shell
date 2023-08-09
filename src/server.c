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

    // Renvoie une liste de tous les résultats
    if (ip[0] == 0) { // '\0' == 0
        rv = getaddrinfo(NULL, port, &hints, &res);
    } else {
        rv = getaddrinfo(ip, port, &hints, &res);
    }
    if (rv != 0) {
        close(socketFD);
        printf("getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }
    // Lier à la première adresse possible
    for (binding = res; res != NULL; res = res->ai_next) {
        if (bind(socketFD, binding->ai_addr, binding->ai_addrlen) == -1) {
            continue;
        } else {
            freeaddrinfo(res); // Le liant pointe vers res, les deux sont donc libérés
            return 0;
        }

    }
    return -1;
}
int acceptConnections(int socketFD) {
    struct sockaddr_in* peer = malloc(sizeof(struct sockaddr_in*)); 
    if (peer == NULL ) {
        return 0;
        }
    socklen_t peer_addr_size = sizeof(peer);
    int connectedSocketFD = accept(socketFD, (struct sockaddr*)peer, &peer_addr_size);
    free(peer);
    return connectedSocketFD;
}

void execShell(char* command, char* rv, long max) {
    char* str = malloc(1);
    if (str == NULL) {
        rv = NULL;
        return;
    }
    FILE* shellReturn;
    shellReturn = popen(command, "r"); // Exécuter la commande
    while (fgets(str, max, shellReturn) != NULL) { 
        // Lire la sortie de la commande dans rv
        strcat(rv, str); 
    }
    free(str);
    return;
}
