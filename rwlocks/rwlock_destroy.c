#include "my_rwlock.h"
#include <asm-generic/errno-base.h>
#include <pthread.h>

int mypthread_rwlock_destroy(mypthread_rwlock_t *rw) {
    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    if (rw->rw_refcount != 0 || rw->rw_nwait_readers != 0 ||
        rw->rw_nwait_writers != 0) {
        return EBUSY;
    }

    pthread_mutex_destroy(&rw->rw_mutex);
    pthread_cond_destroy(&rw->rw_cond_readers);
    pthread_cond_destroy(&rw->rw_cond_writers);

    rw->rw_magic = 0;

    return 0;
}
