#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT "6969"
#define MAX_QUEUE 5

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char ** argv) {
    struct addrinfo hints, *res;
    int sockfd;

    // get socket info

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int r;
    if ((r = getaddrinfo(NULL, PORT, &hints, &res)) == - 1) {
        printf("error: %d\n", errno);
        perror("addr info");
        return -1;
    }

    // make a socket

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // bind to socket

    if ((r = bind(sockfd, res->ai_addr, res->ai_addrlen)) == - 1) {
        printf("error: %d\n", errno);
        perror("bind");
        return -1;
    }
    freeaddrinfo(res);

    // listen in socket
    printf("listening:...\n");
    if ((r = listen(sockfd, MAX_QUEUE)) == - 1) {
        printf("error: %d\n", errno);
        perror("listen");
        return -1;
    }
    
    // accept connect
    
    while (1) {
        struct sockaddr entry_info;
        socklen_t addr_len = sizeof(entry_info);

        int new_fd = accept(sockfd, &entry_info, &addr_len);
        if (new_fd == -1) {
            printf("error: %d\n", errno);
            perror("accept");
            return -1;
        }

        char s[INET6_ADDRSTRLEN];
        inet_ntop(entry_info.sa_family, get_in_addr((struct sockaddr *)&entry_info), s, sizeof(s)); 
        printf("got a connection from %s\n", s); 

        char buff[200];
        int buf_len = sizeof(buff);

        if (recv(new_fd, buff, buf_len, 0) == - 1) {
            printf("error: %d\n", errno);
            perror("recv");
            return -1;
        }

        buff[buf_len] = '\0';

        if (strcmp(buff, "quit") == 0){
            printf("closing server");
            close(new_fd);
            close(sockfd);
            exit(0);
        }

        printf("received: %s\n", buff);
        memset(&buff, 0, sizeof(buff));
        close(new_fd);
    }
    close(sockfd);
    return 0;
}
