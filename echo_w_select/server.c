#include "../lib/headers.h"

#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in client_addr, server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5000);

    Bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    int listenfd = listen(sockfd, 10);

    if (listenfd < 0) {
        perror("listen");
        return -1;
    }

    int maxfd = listenfd;
    int maxi = -1;
    int connected_client_fd[FD_SETSIZE]; // however many fds are allowed
    fd_set allset, rset;

    char buf[1024] = "From Server: ";
    int n;
    size_t start_from = strlen(buf);

    int i = 0;
    for (i = 0; i < FD_SETSIZE; i++) {
        // -1 indicates available entries
        connected_client_fd[i] = -1;
    }

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while (1) {
        rset = allset;

        int nready = select(maxfd + 1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &rset)) {
            // new client connection
            socklen_t client_len = sizeof(client_addr);

            int connfd =
                accept(sockfd, (struct sockaddr *)&client_addr, &client_len);

            for (i = 0; i < FD_SETSIZE; i++) {
                if (connected_client_fd[i] < 0) {
                    connected_client_fd[i] = connfd;
                    break;
                }
            }

            if (i == FD_SETSIZE) {
                err_quit("Too many clients");
            }

            FD_SET(connfd, &allset);

            if (connfd > maxfd) {
                maxfd = connfd;
            }

            if (i > maxi) {
                // for index in connected_client_fd[] array
                maxi = i;
            }

            if (--nready <= 0) {
                // no more readable descriptors
                continue;
            }
        }

        for (i = 0; i <= maxi; i++) {
            int connfd;

            if ((connfd = connected_client_fd[i]) < 0) {
                continue;
            }

            if (FD_ISSET(connfd, &rset)) {
                if ((n = read(connfd, buf + start_from,
                              sizeof(buf) - start_from)) == 0) {
                    // connection closed by client
                    close(connfd);
                    FD_CLR(connfd, &allset);
                    connected_client_fd[i] = -1;
                } else {
                    Writen(connfd, buf, n);
                }

                if (--nready <= 0) {
                    // no more readable descriptors
                    break;
                }
            }
        }
    }
}
