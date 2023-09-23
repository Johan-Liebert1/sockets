#include <asm-generic/errno-base.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO1 "/tmp/fifo1"
#define FIFO2 "/tmp/fifo2"
#define PERM 0666

void client(int readfd, int writefd) {
    char file_name[256];

    if (fgets(file_name, 256, stdin) == NULL) {
        printf("fgets error\n");
        exit(1);
    }

    write(writefd, file_name, strlen(file_name));

    int n = read(readfd, file_name, 256);
    file_name[n] = '\0';
    printf("Client: Read '%d' bytes and '%s' from pipe\n", n, file_name);
}

void server(int readfd, int writefd) {
    char file_name[256];
    int n = read(readfd, file_name, 256);

    // get rid of the new line
    file_name[n - 1] = '\0';

    printf("Server: Read '%d' bytes and '%s' from pipe\n", n, file_name);

    int fd = open(file_name, O_RDONLY);

    if (fd < 0) {
        char *msg = "Failed to open file";
        write(writefd, msg, strlen(msg));
        exit(1);
    }

    char file_contents[256];
    n = read(fd, file_contents, 256);

    if (n < 0) {
        char *msg = "Failed to read from file";
        write(writefd, msg, strlen(msg));
        exit(1);
    }

    write(writefd, file_contents, strlen(file_contents));
}

int main() {
    int fifo1 = mkfifo(FIFO1, PERM);

    // if the file exists, then alls good
    if (fifo1 < 0 && errno != EEXIST) {
        perror("fifo1");
        exit(1);
    }

    int fifo2 = mkfifo(FIFO2, PERM);

    // if the file exists, then alls good
    if (fifo2 < 0 && errno != EEXIST) {
        unlink(FIFO1);

        perror("fifo2");
        exit(1);
    }

    printf("fifo1: %d, fifo2: %d\n", fifo1, fifo2);

    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        exit(1);
    }

    int readfd, writefd;

    if (child_pid > 0) {
        // open the first fifo for read only
        if ((readfd = open(FIFO1, O_RDONLY)) < 0) {
            perror("Parent can't open for reading");
            exit(1);
        }

        if ((writefd = open(FIFO2, O_WRONLY)) < 0) {
            perror("Parent can't open for writing");
            exit(1);
        }

        server(readfd, writefd);

        close(readfd);
        close(writefd);

        unlink(FIFO1);
        unlink(FIFO2);
    } else {
        // open the first fifo for write only
        // we need to do it in this order, else if we opened the
        // fifo2 first for read, then it'll block and the parent is
        // already blocked by its call to read on FIFO1
        if ((writefd = open(FIFO1, O_WRONLY)) < 0) {
            perror("Child can't open for writing");
            exit(1);
        }

        if ((readfd = open(FIFO2, O_RDONLY)) < 0) {
            perror("Child can't open for reading");
            exit(1);
        }

        client(readfd, writefd);

        close(readfd);
        close(writefd);
    }
}
