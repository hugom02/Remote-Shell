#include "headers/client.h"
#include <errno.h>

int main(int argc, char* argv[]) {
    // Check for command line parameters
    int isVerbose = 0;
    char customPort[5];
    char targetIP[15] = {0};
    long maxBytes = MAX;
    strcpy(customPort, PORT);
