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
    if (isVerbose) {
        printf("Serveur démarré avec les paramètres suivants\n");
        if (targetIP[0] == 0) {
            printf("Acceptation de toutes les connexions sur le port %s\n", customPort);
        } else {
           printf("Acceptation de toutes les connexions sur l'IP %s:%s\n", targetIP, customPort);
        }
        printf("Octets max/communication : %ld\n", maxBytes);
    }
    
    // Démarrer l'écoute des connexions
    if (listen(boundSocket, BACKLOG) == -1) {
        if (isVerbose) {
            printf("Erreur lors de l'écoute\n");
        }
        return 0;
    } else {
        if (isVerbose) {
            printf("Écoute démarrée\n");
        }
    }

    // Initialisation des variables nécessaires à la communication
    int peerSocket;
    int bytesReceived;
    int bytesSent;
    void* recvBuf = malloc(maxBytes);
    void* sendBuf = malloc(maxBytes);
    if (recvBuf == NULL || sendBuf == NULL) {
        if (isVerbose) {
            printf("L'appel à malloc() a échoué\n");
        }
        return 0;
    }
    
     // Boucle d'acceptation
    while((peerSocket = acceptConnections(boundSocket))) {
        // Vérifie si un client tente de se connecter,
        // et si la tentative échoue, attend une seconde
        if (peerSocket == -1) {
            if (isVerbose) {
                printf("Échec lors de l'acceptation de la connexion du client\n");
            }
            sleep(1);
            continue;
        }
        if (isVerbose) {
            printf("Connexion acceptée\n");
        }

        // Recevoir le message du client. Si l'envoi du message échoue,
        // fermer la connexion et attendre une seconde
        bytesReceived = recv(peerSocket, recvBuf, maxBytes, 0);

        if (bytesReceived == -1) {
            if (isVerbose) {
                printf("Échec lors de la réception du message\n");
            }
            close(peerSocket);
            sleep(1);
            continue;

        } else {
            if (bytesReceived == 0) {
                // Si aucun octet n'est envoyé, fermer la connexion
                if (isVerbose) {
                    printf("Connexion avec le client fermée\n");
                }
                close(peerSocket);
                continue;
            } else {
                // Exécuter la commande sur l'interpréteur local
                if (isVerbose) {
                    printf("%d octets reçus du client\n", bytesReceived);
                }
                bytesReceived = 0;

                // Supprimer les sauts de ligne en fin de saisie
                char checkExit[strlen((char*)recvBuf)];
                strcpy(checkExit, (char*)recvBuf);
                checkExit[strcspn(checkExit, "\r\n")] = '\0';

                if (strcmp("quitter", checkExit) == 0) {
                    free(recvBuf);
                    free(sendBuf);
                    close(peerSocket);
                    close(boundSocket);
                    if (isVerbose) {
                        printf("Fermeture du serveur\n");
                    }
                    return 1;
                } else {
                    execShell((char*)recvBuf, (char*)sendBuf, maxBytes);
                    if (sendBuf == NULL) {
                        if (isVerbose) {
                            printf("Échec de l'exécution de la commande\n");
                        }
                        continue;
                    }
                    if (isVerbose) {
                        printf("Résultat de la commande :\n\n%s\n", (char*)sendBuf);
                    }

                    // Envoyer les résultats de l'exécution au client
                    bytesSent = send(peerSocket, sendBuf, strlen(sendBuf), 0);
                    if (isVerbose) {
                        printf("%d octets envoyés au client\n", bytesSent);
                    }
                    memset(sendBuf, 0, strlen(sendBuf));
                    memset(recvBuf, 0, strlen(recvBuf));
                }
            }
        }
    }
}
