#include "my_rwlock.h"
#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <pthread.h>

static void rwlock_cancel_wrwait(void *arg) {
    printf("rwlock_cancel_wrwait\n");

    mypthread_rwlock_t *rw = arg;

    rw->rw_nwait_writers--;
    pthread_mutex_unlock(&rw->rw_mutex);
}

int mypthread_rwlock_wrlock(mypthread_rwlock_t *rw) {
    int result;

    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    if ((result = pthread_mutex_lock(&rw->rw_mutex)) != 0) {
        return result;
    }

    while (rw->rw_refcount != 0) {
        rw->rw_nwait_writers++;

        pthread_cleanup_push(rwlock_cancel_wrwait, (void *)rw);
        result = pthread_cond_wait(&rw->rw_cond_writers, &rw->rw_mutex);
        pthread_cleanup_pop(0);

        rw->rw_nwait_writers--;

        if (result != 0) {
            break;
        }
    }

    if (result == 0) {
        // writer has a lock
        rw->rw_refcount = -1;
    }

    pthread_mutex_unlock(&rw->rw_mutex);

    return result;
}
