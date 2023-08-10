#include "headers/client.h"
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
                char arg[3]; // -p=2048 2048
                memcpy(arg, argv[i], 2);
                if (strcmp(arg, "-p") == 0) {
                    char argCopy[strlen(argv[i])]; // -p=2048
                    strcpy(argCopy, argv[i]);
                    char token[5];
                    strcpy(token, strtok(argCopy, "=")); // -p
                    strcpy(token, strtok(NULL, "=")); // 2048
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
                    char token[5];
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
    if (isVerbose) {
        printf("Connexion à l'interpréteur distant avec les paramètres suivants\n");
        if (targetIP[0] == 0) {
            printf("Port : %s\n", customPort);
        } else {
            printf("Adresse IP : %s:%s\n", targetIP, customPort);
        }
        printf("Octets max/communication : %ld\n\n", maxBytes);
    }
    // Initialisation des variables nécessaires à la communication
    void* recvBuf = malloc(maxBytes);
    void* sendBuf = malloc(maxBytes);
    if (recvBuf == NULL || sendBuf == NULL) {
        if (isVerbose) {
            printf("L'appel à malloc() a échoué\n");
        }
        return 0;
    }
    
    int serverSocketFD;
    int bytesSent;
    int bytesReceived;

    // Boucle de connexion
    while((serverSocketFD = connectToServer(customPort, targetIP))) {
        // Vérifie si la connexion est réussie, sinon attend une seconde avant de réessayer
        if (serverSocketFD == -1) {
            if (isVerbose) {
                printf("L'appel à getaddrinfo() a échoué\n");
            }
            sleep(1);
            continue;
        } else if (serverSocketFD == -2) {
            if (isVerbose) {
                printf("Aucun serveur n'a pu être trouvé\n");
            }
            sleep(1);
            continue;
        } else if (serverSocketFD == -3) {
            if (isVerbose) {
                printf("Échec de création de la socket\n");
            }
            sleep(1);
            continue;
        } else {
            printf("Connexion au serveur réussie\n");
        }

        // Obtenir la commande à exécuter sur l'interpréteur distant
        if (isVerbose) {
            printf("Entrez une commande\n");
        }
        fgets((char*)sendBuf, maxBytes, stdin);
        // Envoyer la commande à l'interpréteur distant
        if (isVerbose) {
            printf("Envoi de la commande\n");
        }
        bytesSent = send(serverSocketFD, sendBuf, strlen(sendBuf), 0);
        if (bytesSent == -1) {
            if (isVerbose) {
                printf("Échec de l'envoi\n");
            }
            close(serverSocketFD);
            continue;
        } else {
            if (isVerbose) {
                printf("%d octets envoyés\n", bytesSent);
            }
            bytesSent = 0;
        }
        // Recevoir le résultat de l'exécution depuis l'interpréteur distant
        bytesReceived = recv(serverSocketFD, recvBuf, maxBytes, 0);
        if (bytesReceived == -1) {
            if (isVerbose) {
                printf("Échec de la réception\n");
            }
            close(serverSocketFD);
            continue;
        } else {
            if (isVerbose) {
                printf("%d octets reçus\n", bytesReceived);
            }
        }

        // Supprimer les sauts de ligne en fin de saisie
        char checkExit[strlen((char*)sendBuf)];
        strcpy(checkExit, (char*)sendBuf);
        checkExit[strcspn(checkExit, "\r\n")] = '\0'; 

        if (strcmp(checkExit, "quitter") == 0) {
            if (isVerbose) {
                printf("Serveur fermé\n");
            }
            // Fermeture de l'interpréteur 
            free(recvBuf);
            free(sendBuf);
            close(serverSocketFD);
            return 1;
        }

        // Afficher l'exécution dans l'interpréteur local
        printf("\n%s\n", (char*)recvBuf);
        memset(sendBuf, 0, strlen((char*)sendBuf));
        memset(recvBuf, 0, strlen((char*)recvBuf));
        close(serverSocketFD);
    }
}
