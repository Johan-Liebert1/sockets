#include "../lib/headers.h"
#include "my_rwlock.h"
#include <asm-generic/errno-base.h>
#include <pthread.h>

int mypthread_rwlock_unlock(mypthread_rwlock_t *rw) {
    int result;

    if (rw->rw_magic != RW_MAGIC) {
        return EINVAL;
    }

    if ((result = pthread_mutex_lock(&rw->rw_mutex)) != 0) {
        return result;
    }

    if (rw->rw_refcount > 0) {
        // releasing a reader
        rw->rw_refcount--;
    } else if (rw->rw_refcount == -1) {
        // releasing a writer
        rw->rw_refcount = 0;
    } else {
        err_quit("rw_refcount = %d", rw->rw_refcount);
    }

    // here we give preference to waiting writers over waiting readers
    if (rw->rw_nwait_writers > 0) {
        if (rw->rw_refcount == 0) {
            result = pthread_cond_signal(&rw->rw_cond_writers);
        }
    } else if (rw->rw_nwait_readers > 0) {
        // broadcast as there could be multiple readers waiting for a lock
        result = pthread_cond_broadcast(&rw->rw_cond_readers);
    }

    pthread_mutex_unlock(&rw->rw_mutex);

    return result;
}
