#include <pthread.h>
#include <stdio.h>

#define MAXITEMS 100000
#define MAXTHREADS 10

int buff[MAXITEMS];

struct {
    pthread_mutex_t mutex;
    int nput; // next index to store
    int nval; // next value to store
} put = {PTHREAD_MUTEX_INITIALIZER};

struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int nready;
} nready = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};

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

    // start then wait for the consumer thread
    pthread_create(&tid_consume, NULL, consume, NULL);

    // wait for all the producer threads
    for (int i = 0; i < MAXTHREADS; i++) {
        pthread_join(tid_produce[i], NULL);

        printf("count[%d] = %d\n", i, count[i]);
    }

    pthread_join(tid_consume, NULL);
}

void *produce(void *arg) {
    while (1) {
        pthread_mutex_lock(&put.mutex);

        if (put.nput >= MAXITEMS) {
            pthread_mutex_unlock(&put.mutex);
            return NULL;
        }

        buff[put.nput] = put.nval;

        put.nput++;
        put.nval++;

        pthread_mutex_unlock(&put.mutex);

        pthread_mutex_lock(&nready.mutex);
        if (nready.nready == 0) {
            pthread_cond_signal(&nready.cond);
        }

        nready.nready++;
        pthread_mutex_unlock(&nready.mutex);

        *((int *)arg) += 1;
    }
}

void *consume(void *arg) {
    for (int i = 0; i < MAXITEMS; i++) {
        pthread_mutex_lock(&nready.mutex);

        while (nready.nready == 0) {
            // this does two things atomically
            // 1. the mutex nready.mutex is unlocked
            // 2. the thread is put to sleep until some other thread calls pthread_cond_signal
            // for this condition variable
            //
            // before returning, this will also lock the nready.mutex mutex
            pthread_cond_wait(&nready.cond, &nready.mutex);
        }

        nready.nready--;
        pthread_mutex_unlock(&nready.mutex);

        if (buff[i] != i) {
            printf("buff[%d] = %d\n", i, buff[i]);
        }
    }

    return NULL;
}
