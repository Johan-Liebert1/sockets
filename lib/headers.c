#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

void err_quit(char *msg, ...) {
    va_list args;
    va_start(args, msg);

    vfprintf(stderr, msg, args);

    exit(EXIT_FAILURE);
}

void err_sys(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void println(char *msg, ...) {
    va_list args;
    va_start(args, msg);

    char buf[strlen(msg) + 1];
    sprintf(buf, "%s\n", msg);

    vfprintf(stdout, buf, args);
}

int Socket(int domain, int type, int protocol) {
    int n;

    if ((n = socket(domain, type, protocol)) < 0) {
        err_sys("Socket error");
    }

    return n;
}

int Bind(int fd, const struct sockaddr *addr, socklen_t len) {
    int n;

    if ((n = bind(fd, addr, len)) < 0) {
        err_sys("Bind error");
    }

    return n;
}
