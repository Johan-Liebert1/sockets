#include "../lib/headers.h"
#include <signal.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static int dgrams_received = 0;

static void handle_interrupt(int signo) {
    printf("Received %d datagrams\n", dgrams_received);
    exit(0);
}

int main() {
    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr, client_addr;
    int len;
    char buf[MAXLINE];

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    signal(SIGINT, handle_interrupt);

    while (1) {
        len = sizeof(client_addr);

        recvfrom(sockfd, buf, MAXLINE, 0, (struct sockaddr *)&client_addr,
                 (socklen_t *)&len);

        dgrams_received++;
    }
}

