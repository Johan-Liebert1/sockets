#include "headers.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

const int debug = 1;

/* Write "n" bytes to a descriptor. */
ssize_t writen(int fd, const void *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = vptr;
    nleft = n;

    if (debug) {
        printf("nleft %zu, nwritten %zd, ptr %s\n", nleft, nwritten, ptr);
    }

    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            // Interrupted syscall
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0; /* and call write() again */
            else
                return (-1); /* error */
        }

        nleft -= nwritten;
        ptr += nwritten;

        if (debug) {
            println("nwritten %d. In while", nwritten);
        }
    }

    if (debug) {
        println("Returning from nwritten %d", nwritten);
    }

    return (n);
}

void Writen(int fd, void *ptr, size_t nbytes) {
    if (writen(fd, ptr, nbytes) != nbytes) {
        err_sys("writen error");
    }
}
