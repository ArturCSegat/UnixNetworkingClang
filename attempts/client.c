#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
    
    if (argc != 2) {
        printf("must pass in a msg\n");
        exit(1);
    }

    struct addrinfo hints, *res;
    int sockfd;

    // first, load up address structs with getaddrinfo():

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, "6969", &hints, &res) == - 1) {
        printf("error: %d\n", errno);
        perror("Message from perror");
        return -1;
    }

    // make a socket:

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // connect!

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == - 1) {
        printf("error: %d\n", errno);
        perror("Message from perror");
        return -1;
    }
    freeaddrinfo(res);

    // send to socket
    
    char * msg = argv[1];
    
    if (send(sockfd, msg, strlen(msg), 0) == - 1) {
        printf("error: %d\n", errno);
        perror("Message from perror");
        return -1;
    }
    
    printf("msg sent\n");
    return 0;
}
