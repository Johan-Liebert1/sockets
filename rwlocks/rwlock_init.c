#include "my_rwlock.h"
#include <asm-generic/errno-base.h>
#include <pthread.h>

int mypthread_rwlock_init(mypthread_rwlock_t *rw, pthread_rwlockattr_t *attr) {
    if (attr != NULL) {
        return EINVAL;
    }

    int result;

    if ((result = pthread_mutex_init(&rw->rw_mutex, NULL)) != 0) {
        goto err1;
    }

    if ((result = pthread_cond_init(&rw->rw_cond_readers, NULL)) != 0) {
        goto err2;
    }

    if ((result = pthread_cond_init(&rw->rw_cond_writers, NULL)) != 0) {
        goto err3;
    }

    rw->rw_nwait_readers = 0;
    rw->rw_nwait_writers = 0;
    rw->rw_refcount = 0;
    rw->rw_magic = RW_MAGIC;

err3:
    pthread_cond_destroy(&rw->rw_cond_readers);
err2:
    pthread_cond_destroy(&rw->rw_cond_writers);
err1:
    return result;
}
