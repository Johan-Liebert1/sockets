#include "../lib/headers.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int sockfd, n;
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if (argc != 2)
        err_quit("usage: %s <IPaddress>", argv[0]);

    println("after first if");

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_sys("socket error");
    }

    println("after seond if");

    bzero(&servaddr, sizeof(servaddr));

    println("after bzero");

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(13); 

    println("after servaddr");

    int pton = inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    println("sin_family %d", servaddr.sin_family);
    println("sin_port %d", servaddr.sin_port);
    println("sin_addr %d", servaddr.sin_addr.s_addr);

    if (pton <= 0) {
        err_quit("inet_pton error for %s", argv[1]);
    }

    println("just before connect");

    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
       err_sys("connect error");
    }

    println("after connect");

    while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
        println("n = %d", n);
        recvline[n] = 0; /* null terminate */

        if (fputs(recvline, stdout) == EOF)
            err_sys("fputs error");
    }

    if (n < 0)
        err_sys("read error");

    exit(0);
}
