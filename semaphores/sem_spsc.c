#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#define BUF_SIZE 10
#define SEM_MUTEX "mutex"
#define SEM_NEMPTY "nempty"
#define SEM_NSTORED "nstored"

int nitems = 5000000;

struct {
    int buff[BUF_SIZE];
    sem_t *mutex, *nempty, *nstored;
} shared;

void *produce() {
    for (int i = 0; i < nitems; i++) {
        // wait for at least 1 empty slot
        sem_wait(shared.nempty);
        // sem_wait(shared.mutex);

        shared.buff[i % BUF_SIZE] = i;

        // sem_post(shared.mutex);
        // 1 more stored item
        sem_post(shared.nstored);
    }

    return NULL;
}

void *consume() {
    for (int i = 0; i < nitems; i++) {
        // wait for at least 1 stored item
        sem_wait(shared.nstored);
        // sem_wait(shared.mutex);

        if (shared.buff[i % BUF_SIZE] != i) {
            printf("buff[%d] = %d\n", shared.buff[i], i);
        }

        // sem_post(shared.mutex);
        // 1 more empty slot
        sem_post(shared.nempty);
    }

    return NULL;
}

int main() {
    shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 400, 1);
    shared.nempty = sem_open(SEM_NEMPTY, O_CREAT | O_EXCL, 400, BUF_SIZE);
    shared.nstored = sem_open(SEM_NSTORED, O_CREAT | O_EXCL, 400, 0);

    pthread_t tid_producer, tid_consumer;

    pthread_create(&tid_producer, NULL, produce, NULL);
    pthread_create(&tid_consumer, NULL, consume, NULL);

    pthread_join(tid_producer, NULL);
    pthread_join(tid_consumer, NULL);

    // remove the semaphores
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_NEMPTY);
    sem_unlink(SEM_NSTORED);
}
