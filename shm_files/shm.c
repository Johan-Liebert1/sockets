#include "../lib/headers.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

static int num_loops = 10;

int main() {
    int count = 0;
    sem_t mutex;

    if (sem_init(&mutex, 1, 1) == -1) {
        err_sys("sem_init");
    }

    int fd = open("count", O_CREAT | O_RDWR, 444);

    if (fd < 0) {
        err_sys("open");
    }

    write(fd, &count, sizeof(int));

    int *ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    close(fd);

    if (ptr == NULL) {
        err_sys("mmap");
    }

    if (fork() == 0) {
        for (int i = 0; i < num_loops; i++) {
            sem_wait(&mutex);
            printf("Child: %d\n", (*ptr)++);
            sem_post(&mutex);
        }

        exit(0);
    } 

    for (int i = 0; i < num_loops; i++) {
        sem_wait(&mutex);
        printf("Parent: %d\n", (*ptr)++);
        sem_post(&mutex);
    }
}
