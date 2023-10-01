#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define NUM_PRODUCERS 2
#define NUM_BUFF 2

int nitems = BUF_SIZE;
int fd;

struct {
    struct {
        char data[BUF_SIZE];
        ssize_t n;
    } buff[NUM_BUFF];
    sem_t mutex, nempty, nstored;
} shared;

void *produce(void *arg) {
    for (int i = 0;;) {
        // wait for at least 1 empty slot
        sem_wait(&shared.nempty);

        shared.buff[i].n = read(fd, shared.buff[i].data, BUF_SIZE);

        if (shared.buff[i].n == 0) {
            sem_post(&shared.nstored);
            return NULL;
        }

        // treat as circular buffer
        if (++i >= NUM_BUFF)
            i = 0;

        sem_post(&shared.nstored);
    }

    return NULL;
}

void *consume() {
    for (int i = 0;;) {
        sem_wait(&shared.nstored);

        if (shared.buff[i].n == 0) {
            return NULL;
        }

        write(fileno(stdout), shared.buff[i].data, shared.buff[i].n);

        if (++i >= NUM_BUFF)
            i = 0;

        sem_post(&shared.nempty);
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: ./out <file_name>\n");
        return 1;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("open");
        return 1;
    }

    sem_init(&shared.mutex, 0, 1);
    sem_init(&shared.nempty, 0, BUF_SIZE);
    sem_init(&shared.nstored, 0, 0);

    pthread_t tid_producer, tid_consumer;
    int count[NUM_PRODUCERS];

    pthread_create(&tid_producer, NULL, produce, NULL);
    pthread_create(&tid_consumer, NULL, consume, NULL);

    pthread_join(tid_producer, NULL);
    pthread_join(tid_consumer, NULL);

    // remove the semaphores
    sem_destroy(&shared.mutex);
    sem_destroy(&shared.nempty);
    sem_destroy(&shared.nstored);
}
