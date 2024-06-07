#include <pthread.h>
#include <semaphore.h>

typedef struct _rwwlock_t {
    pthread_mutex_t mutex;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;
    int readers;
    int writers;
    int waiting_writers;
} rwwlock_t;

void rwwlock_init(rwwlock_t *rww) {
    pthread_mutex_init(&rww->mutex, NULL);
    pthread_cond_init(&rww->readers_cond, NULL);
    pthread_cond_init(&rww->writers_cond, NULL);
    rww->readers = 0;
    rww->writers = 0;
    rww->waiting_writers = 0;
}

void rwwlock_acquire_writelock(rwwlock_t *rww) {
    pthread_mutex_lock(&rww->mutex);
    rww->waiting_writers++;
    while (rww->readers > 0 || rww->writers > 0) {
        pthread_cond_wait(&rww->writers_cond, &rww->mutex);
    }
    rww->waiting_writers--;
    rww->writers++;
    pthread_mutex_unlock(&rww->mutex);
}

void rwwlock_release_writelock(rwwlock_t *rww) {
    pthread_mutex_lock(&rww->mutex);
    rww->writers--;
    if (rww->waiting_writers > 0) {
        pthread_cond_signal(&rww->writers_cond);
    } else {
        pthread_cond_broadcast(&rww->readers_cond);
    }
    pthread_mutex_unlock(&rww->mutex);
}

void rwwlock_acquire_readlock(rwwlock_t *rww) {
    pthread_mutex_lock(&rww->mutex);
    while (rww->writers > 0 || rww->waiting_writers > 0) {
        pthread_cond_wait(&rww->readers_cond, &rww->mutex);
    }
    rww->readers++;
    pthread_mutex_unlock(&rww->mutex);
}

void rwwlock_release_readlock(rwwlock_t *rww) {
    pthread_mutex_lock(&rww->mutex);
    rww->readers--;
    if (rww->readers == 0 && rww->waiting_writers > 0) {
        pthread_cond_signal(&rww->writers_cond);
    }
    pthread_mutex_unlock(&rww->mutex);
}