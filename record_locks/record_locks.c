#include "../lib/headers.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void lock_file(int fd) {
    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;

    // lock the entire file
    lock.l_start = 0;
    lock.l_len = 0;
    
    fcntl(fd, F_SETLKW, &lock);
}

void unlock_file(int fd) {
    struct flock lock;

    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;

    // unlock the entire file
    lock.l_start = 0;
    lock.l_len = 0;
    
    fcntl(fd, F_SETLK, &lock);
}

int main() {
    int fd = open("print.txt", O_RDWR);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    char line[MAXLINE];
    long seqno;

    for (int i = 0; i < 20000; i++) {
        lock_file(fd);

        // rewind before read
        lseek(fd, 0L, SEEK_SET);

        int n = read(fd, line, MAXLINE);

        if (n < 0) {
            perror("read");
            return 1;
        }

        line[n] = '\0'; // null terminate for scanf
        
        n = sscanf(line, "%ld\n", &seqno);

        if (n < 0) {
            perror("scanf");
            return 1;
        }

        // printf("pid = %d, seqno = %ld\n", getpid(), seqno);

        seqno++;

        snprintf(line, sizeof(line), "%ld\n", seqno);
        lseek(fd, 0, SEEK_SET);
        write(fd, line, strlen(line));

        unlock_file(fd);
    }
}
