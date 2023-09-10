#include "../lib/headers.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int sockfd, n;
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("usage: %s <IPaddress>", argv[0]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_sys("socket error");
    }

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13);

    int pton = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    println("sin_family %d", servaddr.sin_family);
    println("sin_port %d", servaddr.sin_port);
    println("sin_addr %d", servaddr.sin_addr.s_addr);

    if (pton <= 0) {
        err_quit("inet_pton error for %s", argv[1]);
    }

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
        err_sys("connect error");
    }

    while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0; /* null terminate */
        printf("Read: %s\n", recvline);
    }

    if (n < 0)
        err_sys("read error");

    exit(0);
}
