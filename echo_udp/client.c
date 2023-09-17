#include "../lib/headers.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

void dg_cli(int sockfd, struct sockaddr *server_addr,
            socklen_t server_addr_len) {
    char buf[MAXLINE];

    while (fgets(buf, MAXLINE, stdin) != NULL) {
        int n =
            sendto(sockfd, buf, strlen(buf), 0, server_addr, server_addr_len);

        if (n < 0) {
            err_sys("sendto");
        }

        n = recvfrom(sockfd, buf, MAXLINE, 0, server_addr, &server_addr_len);

        if (n < 0) {
            err_sys("recvfrom");
        }

        printf("%s\n", buf);
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
