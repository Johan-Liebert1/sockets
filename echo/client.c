#include "../lib/headers.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void take_user_input(FILE *fp, int sockfd) {
    char sendline[1024], recvline[1024];

    while (fgets(sendline, 1024, fp) != NULL) {
        println("in take_user_input while %s", sendline);

        // strlen(sendline) - 1 => to not send \n
        Writen(sockfd, sendline, strlen(sendline) - 1);

        if (read(sockfd, recvline, 1024) == 0) {
            err_quit("take_user_input: server terminated prematurely");
        }

        printf("received: %s\n", recvline);
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
