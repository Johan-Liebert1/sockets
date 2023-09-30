#include "my_rwlock.h"
#include <asm-generic/errno-base.h>
#include <pthread.h>

int mypthread_rwlock_rdlock(mypthread_rwlock_t *rw) {
    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    int result;

    if ((result = pthread_mutex_lock(&rw->rw_mutex)) != 0) {
        return result;
    }

    // give preference to waiting writers
    while (rw->rw_refcount < 0 || rw->rw_nwait_writers > 0) {
        // cannot get the lock yet, so inc the number of waiting readers
        rw->rw_nwait_readers++;
        result = pthread_cond_wait(&rw->rw_cond_readers, &rw->rw_mutex);
        rw->rw_nwait_readers--;

        if (result != 0) break;
    }

    if (result == 0) {
        // we got the lock, i.e. number of read locks increased
        rw->rw_refcount++;
    }

    pthread_mutex_unlock(&rw->rw_mutex);

    return result;
}
