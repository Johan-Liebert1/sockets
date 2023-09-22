#include "../lib/headers.h"
#include <asm-generic/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

void replace_newlines(char *buf) {
    for (int i = 0; i < strlen(buf); i++) {
        if (buf[i] == '\n') {
            buf[i] = '%';
        }
    }
}

void dg_echo(int sockfd, struct sockaddr *client_addr, socklen_t clilen) {
    int n;
    socklen_t len;
    char msg[MAXLINE] = "From UDP server: ";

    int forward_into_msg = strlen(msg);

    while (1) {
        len = clilen;
        n = recvfrom(sockfd, msg + forward_into_msg, MAXLINE - forward_into_msg,
                     0, client_addr, &len);

        if (n == -1) {
            err_sys("recvfrom");
        }

        replace_newlines(msg);
        int sent_bytes = sendto(sockfd, msg, strlen(msg), 0, client_addr, len);

        if (sent_bytes == -1) {
            err_sys("sendto");
        }

        printf("Wrote %d bytes to peer\n", sent_bytes);
    }
}

int main() {
    struct sockaddr_in server_addr, client_addr;

    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        err_sys("setsockopt");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    dg_echo(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr));
}
