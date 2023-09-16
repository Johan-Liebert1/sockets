#include "../lib/headers.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

void take_user_input(FILE *fp, int sockfd) {
    char sendline[1024], recvline[1024];

    fd_set rset;
    int maxfdp1;

    FD_ZERO(&rset);

    while (1) {
        FD_SET(sockfd, &rset);
        FD_SET(fileno(fp), &rset);

        maxfdp1 = MAX(sockfd, fileno(fp)) + 1;

        if (select(maxfdp1, &rset, NULL, NULL, NULL) == -1) {
            err_sys("select");
        }

        if (FD_ISSET(sockfd, &rset)) {
            if (read(sockfd, recvline, 1024) == 0) {
                err_quit("server terminated prematurely");
            }

            fputs(recvline, stdout);
        }

        if (FD_ISSET(fileno(fp), &rset)) {
            if (fgets(sendline, 1024, stdin) == NULL) {
                return;
            }

            write(sockfd, sendline, strlen(sendline));
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        err_quit("usage: %s <IPaddress>", argv[0]);
    }

    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);

    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    println("%d", server_addr.sin_addr.s_addr);

    int conn = connect(sockfd, (SA *)&server_addr, sizeof(server_addr));

    if (conn == -1) {
        perror("connect");
        return 1;
    }

    take_user_input(stdin, sockfd);

    return 0;
}
