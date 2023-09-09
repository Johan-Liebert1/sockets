#include "../lib//headers.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main() {
    struct sockaddr_in servaddr, clientaddr;
    int connfd;
    char buff[MAXLINE];

    int listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(13);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    for (;;) {
        socklen_t len = sizeof(clientaddr);

        // accept creates (the kernel creates) a new socket for each TCP connection
        // if we don't care about the client's info, we can set the last two arguments as NULL
        connfd = accept(listenfd, (SA *)&clientaddr, &len);

        println("connfd %d", connfd);

        time_t ticks = time(NULL);

        println("Connection from %s, Port %d", 
                inet_ntop(AF_INET, &clientaddr.sin_addr, buff, sizeof(buff)),
                ntohs(clientaddr.sin_port)
                );

        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));

        close(connfd);
    }
}
