#include "../lib/headers.h"

#include <errno.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void str_echo(int connfd) {
    char buf[1024] = "From Server: ";
    int n;

    size_t start_from = strlen(buf);

again:
    while ((n = read(connfd, buf + start_from, sizeof(buf) - start_from)) > 0) {
        printf("Received '%s' from client. n = %d\n", buf + start_from, n);
        // Writen(connfd, buf, strlen(buf));
        int written = write(connfd, buf, strlen(buf));

        printf("Wrote %d chars\n", written);
    }

    if (n < 0 && errno == EINTR) {
        goto again;
    } else if (n < 0) {
        err_sys("str_echo: read error");
    }
}

int main() {
    int sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in client_addr, server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(5000);

    Bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    int listener = listen(sockfd, 10);

    if (listener < 0) {
        perror("listen");
        return -1;
    }

    while (1) {
        socklen_t client_len = sizeof(client_addr);

        int connfd =
            accept(sockfd, (struct sockaddr *)&client_addr, &client_len);

        int child_pid = fork();

        if (child_pid == -1) {
            perror("fork");
            return 1;
        }

        if (child_pid == 0) {
            // decrements the file descriptor's reference count by 1
            // does not actually close the socket as it's still open in parent
            close(sockfd);

            str_echo(connfd);
            exit(0);
        } // conn fd closed here

        // close the final listener for connfd
        // the socket created by accept will be cleaned up here
        close(connfd);
    }
}
