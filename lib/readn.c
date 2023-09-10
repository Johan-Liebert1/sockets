#include "headers.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/* Read "n" bytes from a descriptor. */
ssize_t readn(int fd, void *vptr, size_t n) {
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = vptr;
    nleft = n;

    while (nleft > 0) {
        println("in readn. nleft %d", nleft);

        if ((nread = read(fd, ptr, nleft)) < 0) {
            // Interrupted syscall
            if (errno == EINTR) {
                nread = 0; /* and call read() again */
            } else {
                return (-1);
            }
        } else if (nread == 0) {
            break; /* EOF */
        }

        nleft -= nread;
        ptr += nread;
    }

    println("After while readn");

    return (n - nleft); /* return >= 0 */
}
/* end readn */

ssize_t Readn(int fd, void *ptr, size_t nbytes) {
    ssize_t n;

    if ((n = readn(fd, ptr, nbytes)) < 0)
        err_sys("readn error");

    return (n);
}
