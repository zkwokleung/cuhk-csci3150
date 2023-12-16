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
    rwl->readers = rwl->waiting_readers = 0;
    rwl->writer = rwl->waiting_writers = 0;
}

void rwlock_read_lock(struct rwlock *rwl)
{
    pthread_mutex_lock(&rwl->mutex);
    rwl->waiting_readers++;

    /* AREA 1 BEGIN */
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
    rwl->waiting_writers++;

    /* AREA 2 BEGIN */
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

    /* AREA 3 */
    if (!rwl->readers)
        pthread_cond_signal(&rwl->writer_cond);

    /* AREA 4 */
    if (!rwl->waiting_writers && rwl->waiting_readers)
        pthread_cond_signal(&rwl->reader_cond);

    pthread_mutex_unlock(&rwl->mutex);
}

void rwlock_write_unlock(struct rwlock *rwl)
{
    pthread_mutex_lock(&rwl->mutex);
    rwl->writer--;

    /* AREA 5 BEGIN */
    if (rwl->waiting_writers)
        pthread_cond_signal(&rwl->writer_cond);
    else if (rwl->waiting_readers)
        pthread_cond_signal(&rwl->reader_cond);
    /* AREA 5 END */

    pthread_cond_broadcast(&rwl->reader_cond);

    pthread_mutex_unlock(&rwl->mutex);
}