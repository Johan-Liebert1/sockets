#include <pthread.h>
#include <stdio.h>

#define MAXITEMS 100000
#define MAXTHREADS 10

int nitems;

struct {
    pthread_mutex_t mutex;
    int buff[MAXITEMS];
    int nput;
    int nval;
} put = {PTHREAD_MUTEX_INITIALIZER};

void *produce(void *), *consume(void *);

int main() {
    // pthread_setconcurrency();

    int count[MAXTHREADS];

    pthread_t tid_produce[MAXTHREADS], tid_consume;

    // start all producer threads
    for (int i = 0; i < MAXTHREADS; i++) {
        count[i] = 0;
        pthread_create(&tid_produce[i], NULL, produce, &count[i]);
    }

    // wait for all the producer threads
    for (int i = 0; i < MAXTHREADS; i++) {
        pthread_join(tid_produce[i], NULL);

        printf("count[%d] = %d\n", i, count[i]);
    }

    // start then wait for the consumer thread
    pthread_create(&tid_consume, NULL, consume, NULL);
    pthread_join(tid_consume, NULL);
}

void *produce(void *arg) {
    while (1) {
        // pthread_mutex_lock(&shared.mutex);

        if (put.nput >= MAXITEMS) {
            pthread_mutex_unlock(&put.mutex);
            return NULL;
        }

        put.buff[put.nput] = put.nval;
        put.nput++;
        put.nval++;

        // pthread_mutex_unlock(&shared.mutex);

        *((int *)arg) += 1;
    }
}

void* consume(void *arg) {
    for (int i = 0; i < MAXITEMS; i++) {
        if (put.buff[i] != i) {
            printf("buff[%d] = %d\n", i, put.buff[i]);
        }
    }

    return NULL;
}
