#include "../lib//headers.h"
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main() {
    struct sockaddr_in servaddr;
    int connfd;
    char buff[MAXLINE];

    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    println("After connect");

    for (;;) {
        connfd = accept(listenfd, (SA *)NULL, NULL);
        println("After accept");

        println("connfd %d", connfd);

        time_t ticks = time(NULL);

        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));

        println("fuff %s", buff);

        write(connfd, buff, strlen(buff));

        close(connfd);
    }
}
