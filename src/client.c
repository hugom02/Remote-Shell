#include "../headers/client.h"

int connectToServer(char* port, char* ip) {
	// Récupérer l'adresse du serveur
	struct addrinfo hints, *res, *attempt;
	// Initialiser tous les indices à 0
	memset(&hints, 0, sizeof(struct addrinfo));
	int rv;
	int serverSocket;
	// Le serveur est IPv4 et le type de socket est STREAM
    	hints.ai_family = AF_INET; // INET = IPv4
    	hints.ai_socktype = SOCK_STREAM;

	// Renvoie une liste de tous les résultats
	if (ip[0] == 0) {
		rv = getaddrinfo(NULL, port, &hints, &res);
	} else {
		rv = getaddrinfo(ip, port, &hints, &res);
	}
	
	if (rv != 0) {
		return -1;
	}

	// Pas de résultat
	if (res == NULL) {
		return -2;
	}
