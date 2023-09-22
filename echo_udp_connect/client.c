#include "../lib/headers.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void dg_cli(int sockfd, struct sockaddr *server_addr,
            socklen_t server_addr_len) {

    char send_line[MAXLINE];
    char recv_line[MAXLINE];

    // this makes it so that the current UDP socket can only connect to the
    // peer whose address we've passed in to server_addr
    // any messages from other peers to this socket will be discarded
    if (connect(sockfd, server_addr, server_addr_len) == -1) {
        err_sys("connect");
    }

    while (fgets(send_line, MAXLINE, stdin) != NULL) {
        int n = write(sockfd, send_line, strlen(send_line));

        if (n < 0) {
            err_sys("sendto");
        }

        n = read(sockfd, recv_line, MAXLINE);

        if (n < 0) {
            err_sys("recvfrom");
        }

        printf("%s\n", recv_line);
    }
}

int main() {
    struct sockaddr_in server_addr;

    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    inet_pton(AF_INET, LOCALHOST, &server_addr.sin_addr);

    dg_cli(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
}
