#ifndef __mypthread_rwlock_h
#define __mypthread_rwlock_h

#include <pthread.h>

typedef struct {
    // basic lock on this struct
    pthread_mutex_t rw_mutex;

    // for reader threads waiting
    pthread_cond_t rw_cond_readers;
    
    // for writer threads waiting
    pthread_cond_t rw_cond_writers;

    // for error checking
    int rw_magic;
    // the number waiting readers
    int rw_nwait_readers;
    // the number waiting writers
    int rw_nwait_writers;
    // -1 if writer has the lock, else number of readers holding the lock
    int rw_refcount;
} mypthread_rwlock_t;

#define RW_MAGIC 0x19283746

#define MYPTHREAD_RWLOCK_INITIALIZER  { \
    PTHREAD_MUTEX_INITIALIZER, \
    PTHREAD_COND_INITIALIZER, \
    PTHREAD_COND_INITIALIZER, \
    RW_MAGIC, 0, 0, 0 \
}

// function prototypes
int mypthread_rwlock_destroy(mypthread_rwlock_t *);
int mypthread_rwlock_init(mypthread_rwlock_t *, pthread_rwlockattr_t *);
int mypthread_rwlock_rdlock(mypthread_rwlock_t *);
int mypthread_rwlock_wrlock(mypthread_rwlock_t *);
int mypthread_rwlock_unlock(mypthread_rwlock_t *);

#endif /* ifndef __mypthread_rwlock_h */
