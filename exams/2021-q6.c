#include <pthread.h>

struct rwlock
{
    pthread_mutex_t mutex;
    pthread_cond_t reader_cond;
    pthread_cond_t writer_cond;
    int readers, waiting_readers;
    int writer, waiting_writers;
};

void rwlock_init(struct rwlock *rwl)
{
    pthread_mutex_init(&rwl->mutex, NULL);
    pthread_cond_init(&rwl->reader_cond, NULL);
    pthread_cond_init(&rwl->writer_cond, NULL);
    rwl->readers = rwl->waiting_readers = rwl->writer = rwl->waiting_writers = 0;
}

void rwlock_read_lock(struct rwlock *rwl)
{
    pthread_mutex_lock(&rwl->mutex);

    /* AREA 1 BEGIN (if read lock cannot be acquired, wait on the conditional variable) */
    while (rwl->writer || rwl->waiting_writers)
        pthread_cond_wait(&rwl->reader_cond, &rwl->mutex);
    /* AREA 1 END */

    rwl->waiting_readers--;
    rwl->readers++;
    pthread_mutex_unlock(&rwl->mutex);
}

void rwlock_write_lock(struct rwlock *rwl)
{
    pthread_mutex_lock(&rwl->mutex);

    /* AREA 2 BEGIN (if write lock cannot be acquired, wait on the conditional variable) */
    while (rwl->readers || rwl->writer)
        pthread_cond_wait(&rwl->writer_cond, &rwl->mutex);
    /* AREA 2 END */

    rwl->waiting_writers--;
    rwl->writer++;
    pthread_mutex_unlock(&rwl->mutex);
}

void rwlock_read_unlock(struct rwlock *rwl)
{
    pthread_mutex_lock(&rwl->mutex);
    rwl->readers--;

    /* AREA 3 BEGIN (if there are no readers, signal the writer conditional variable) */
    if (!rwl->readers)
        pthread_cond_signal(&rwl->writer_cond);
    /* AREA 3 END */

    /* AREA 4 BEGIN (if there is no waiting writer, and there are waiting readers, then wake up one waiting reader) */
    if (!rwl->waiting_writers && rwl->waiting_readers)
        pthread_cond_signal(&rwl->reader_cond);
    /* AREA 4 END */

    pthread_mutex_unlock(&rwl->mutex);
}

void rwlock_write_unlock(struct rwlock *rwl)
{
    pthread_mutex_lock(&rwl->mutex);
    rwl->writer--;

    /* AREA 5 BEGIN (if there are no waiting writers, and there are waiting readers, then wake up all waiting readers)
     */
    if (!rwl->waiting_writers && rwl->waiting_readers)
        pthread_cond_broadcast(&rwl->reader_cond);
    /* AREA 5 END */

    /* AREA 6 BEGIN (if there are no waiting writers, and there are no waiting readers, then wake up one waiting writer)
     */
    if (!rwl->waiting_writers && !rwl->waiting_readers)
        pthread_cond_signal(&rwl->writer_cond);
    /* AREA 6 END */

    pthread_mutex_unlock(&rwl->mutex);
}