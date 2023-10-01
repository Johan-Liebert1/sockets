#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1
#define NUM_PRODUCERS 2

int nitems = BUF_SIZE;

struct {
    int buff[BUF_SIZE];
    int nput; 
    int nputval;
    sem_t mutex, nempty, nstored;
} shared;

void *produce(void *arg) {
    for (;;) {
        // wait for at least 1 empty slot
        sem_wait(&shared.nempty);
        printf("producer got nempty\n");
        sem_wait(&shared.mutex);
        printf("producer got mutex\n");

        if (shared.nput >= nitems) {
            // all done here
            printf("producer terminating\n");
            printf("producer released mutex\n");
            sem_post(&shared.mutex);
            printf("producer released nempty\n\n");
            sem_post(&shared.nempty);
            return NULL;
        }

        shared.buff[shared.nput % BUF_SIZE] = shared.nputval;
        shared.nput++;
        shared.nputval++;

        sem_post(&shared.mutex);
        printf("producer released mutex\n");
        // 1 more stored item
        sem_post(&shared.nstored);
        printf("producer released nstored\n\n");

        *((int *)arg) += 1;
    }

    return NULL;
}

void *consume() {
    for (int i = 0; i < nitems; i++) {
        // wait for at least 1 stored item

        sem_wait(&shared.nstored);
        printf("consumer gets nstored\n");
        sem_wait(&shared.mutex);
        printf("consumer gets mutex\n");

        if (shared.buff[i % BUF_SIZE] != i) {
            printf("buff[%d] = %d\n", i, shared.buff[i % BUF_SIZE]);
        }

        sem_post(&shared.mutex);
        printf("consumer releases mutex\n");
        // 1 more empty slot
        sem_post(&shared.nempty);
        printf("consumer releases nempty\n\n");
    }

    return NULL;
}

int main() {
    sem_init(&shared.mutex, 0, 1);
    sem_init(&shared.nempty, 0, BUF_SIZE);
    sem_init(&shared.nstored, 0, 0);

    pthread_t tid_producer[NUM_PRODUCERS], tid_consumer;
    int count[NUM_PRODUCERS];

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        count[i] = i;
        pthread_create(&tid_producer[i], NULL, produce, &count[i]);
    }

    pthread_create(&tid_consumer, NULL, consume, NULL);

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(tid_producer[i], NULL);
    }

    printf("After all produceres have returned\n");

    pthread_join(tid_consumer, NULL);
    printf("After all consumers have returned\n");

    // remove the semaphores
    sem_destroy(&shared.mutex);
    sem_destroy(&shared.nempty);
    sem_destroy(&shared.nstored);
}
