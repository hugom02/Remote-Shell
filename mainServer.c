#include "headers/server.h"
#include <errno.h>

int main(int argc, char* argv[]) {
    // Check for command line parameters
    int isVerbose = 0;
    char customPort[5];
    char targetIP[15] = {0};
    long maxBytes = MAX;
    strcpy(customPort, PORT);

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-v") == 0) {
                isVerbose = 1;
            } else {
                char arg[3]; 
                memcpy(arg, argv[i], 2);
                if (strcmp(arg, "-p") == 0) {
                    char argCopy[strlen(argv[i])];
                    strcpy(argCopy, argv[i]);
                    char token[5];
                    strcpy(token, strtok(argCopy, "="));
                    strcpy(token, strtok(NULL, "="));
                    long portLong = strtol(token, NULL, 10);
                    if (portLong < 0 || portLong > 65535 || errno != 0) {
                        printf("Le paramètre doit être entre 0 et 65535\n");
                        return 0;
                    } else {
                        strcpy(customPort, token);
                    }
                } else if (strcmp(arg, "-b") == 0) {
                    char argCopy[strlen(argv[i])];
                    strcpy(argCopy, argv[i]); 
                    char token[4];
                    strcpy(token, strtok(argCopy, "="));
                    strcpy(token, strtok(NULL, "="));
                    long bytesLong = strtol(token, NULL, 10);
                    if (bytesLong < 1 || bytesLong > 10240 || errno != 0) {
                        printf("Le paramètre doit être entre 1 et 10240\n");
                        return 0;
                    } else {
                        maxBytes = bytesLong;
                    }
                } else if (strcmp(arg, "-i") == 0) {
                    char argCopy[strlen(argv[i])];
                    strcpy(argCopy, argv[i]);
                    char token[15]; 
                    strcpy(token, strtok(argCopy, "="));
                    strcpy(token, strtok(NULL, "="));
                    if (inet_addr(token) == -1) {
                        // -1 correspond à une adresse IP valide, mais c'est l'adresse de diffusion
                        printf("Adresse IP invalide\n");
                        return 0;
                    }
                    // TODO: Vérifier les adresses IP réservées
                    strcpy(targetIP, token);
                } else {
                    printf("Les arguments en ligne de commande sont :\n");
                    printf("-v : mode verbeux - affiche tous les messages d'exécution\n");
                    printf("-i=N.N.N.N : définit l'adresse IP du serveur à N.N.N.N (IPv4)\n");
                    printf("-p=N : définit le port du serveur à N (0<N<65535)\n");
                    printf("-b=N : définit les octets maximum par communication (0<N<2048)\n");
                    return 0;
                }
            }
        }
    }

    // Créer et lier la socket
    int boundSocket = createSocket();
    if (boundSocket == -1) {
        if (isVerbose) {
            printf("Erreur lors de la création de la socket\n");
        }
        return 0;
    } else {
        if (isVerbose) {
            printf("Socket créée\n");
        }
    }
    if (bindSocket(boundSocket, customPort, targetIP) == -1) {
        if (isVerbose) {
            printf("Erreur lors de la liaison de la socket\n");
        }
        return 0;
    } else {
        if (isVerbose) {
            printf("Socket liée\n");
        }
    }
