#include "../lib/headers.h"

#include <asm-generic/errno.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define OPEN_MAX 256

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

    int maxi = -1;
    struct pollfd connected_client_fd[OPEN_MAX]; // however many fds are allowed
    fd_set allset, rset;

    connected_client_fd[0].fd = listenfd;
    connected_client_fd[0].events = POLLRDNORM;

    char buf[1024] = "From Server: ";
    int read_bytes;
    size_t start_from = strlen(buf);

    int i = 0;
    for (i = 1; i < OPEN_MAX; i++) {
        // -1 indicates available entries
        connected_client_fd[i].fd = -1;
    }

    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while (1) {
        rset = allset;

        int nready = poll(connected_client_fd, maxi + 1, -1);

        // 0th index is the listen fd
        if (connected_client_fd[0].revents & POLLRDNORM) {
            // new client connection
            socklen_t client_len = sizeof(client_addr);

            int connfd =
                accept(sockfd, (struct sockaddr *)&client_addr, &client_len);

            for (i = 0; i < OPEN_MAX; i++) {
                if (connected_client_fd[i].fd < 0) {
                    connected_client_fd[i].fd = connfd;
                    break;
                }
            }

            if (i == OPEN_MAX) {
                err_quit("Too many clients");
            }

            connected_client_fd[i].events = POLLRDNORM;

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

            if ((connfd = connected_client_fd[i].fd) < 0) {
                continue;
            }

            if (connected_client_fd[i].revents & (POLLRDNORM | POLLERR)) {
                if ((read_bytes = read(connfd, buf + start_from,
                              sizeof(buf) - start_from)) < 0) {
                    if (errno == ECONNRESET) {
                        // connection reset by client
                        close(connfd);
                        connected_client_fd[i].fd = -1;
                    } else {
                        err_sys("read error");
                    }
                } else if (read_bytes == 0) {
                    // connection closed by client
                    close(connfd);
                    connected_client_fd[i].fd = -1;
                } else {
                    Writen(connfd, buf, read_bytes);
                }

                if (--nready <= 0) {
                    // no more readable descriptors
                    break;
                }
            }
        }
    }
}
