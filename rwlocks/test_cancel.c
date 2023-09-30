#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "my_rwlock.h"

mypthread_rwlock_t rwlock = MYPTHREAD_RWLOCK_INITIALIZER;
pthread_t tid1, tid2;

void *thread1(void*), *thread2(void*);

int main() {
    void *status;

    pthread_create(&tid1, NULL, thread1, NULL);
    sleep(1); // let thread1() get the lock
    
    pthread_create(&tid2, NULL, thread2, NULL);

    pthread_join(tid2, &status);
    if (status != PTHREAD_CANCELED) {
        printf("thread2 status = %p\n", status);
    }

    pthread_join(tid1, &status);
    if (status != NULL) {
        printf("thread1 status = %p\n", status);
    }

    printf("rw_nwait_readers = %d\n", rwlock.rw_nwait_readers);
    printf("rw_nwait_writers = %d\n", rwlock.rw_nwait_writers);
    printf("rw_refcount = %d\n", rwlock.rw_refcount);

    mypthread_rwlock_destroy(&rwlock);
    return 0;
}

void *thread1(void *arg) {
    mypthread_rwlock_rdlock(&rwlock);
    printf("thread1() got a read lock\n");
    sleep(3);

    pthread_cancel(tid2);
    sleep(3);

    printf("thread1() released the read lock\n");
    mypthread_rwlock_unlock(&rwlock);
    return NULL;
}


void *thread2(void *arg) {
    printf("thread2() trying to obtain a write lock\n");

    mypthread_rwlock_wrlock(&rwlock);
    printf("thread2() got a write lock\n"); // should not get here

    sleep(1);
    mypthread_rwlock_unlock(&rwlock);

    return NULL;
}
