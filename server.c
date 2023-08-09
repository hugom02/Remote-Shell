#include "../headers/server.h"

int createSocket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}
