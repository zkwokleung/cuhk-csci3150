#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 10

struct scl
{
    int total_lock_hold_time;
    pthread_mutex_t mutex;
};

int lock_hold_time[NUM_THREADS];
int crit_section_begin_time[NUM_THREADS];

int get_ticks()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void scl_lock(int thread_id, struct scl *lock)
{
    // BOX1
    int quota = lock->total_lock_hold_time / NUM_THREADS;

    // BOX2
    if (lock_hold_time[thread_id] >= quota)
    {
        // BOX3 & BOX4
        int wait_time = lock_hold_time[thread_id] - quota;
        sleep(wait_time);
    }

    pthread_mutex_lock(&lock->mutex);
    crit_section_begin_time[thread_id] = get_ticks();
}

void scl_unlock(int thread_id, struct scl *lock)
{
    pthread_mutex_unlock(&lock->mutex);
    // BOX5
    int time_spent_in_critical_section = get_ticks() - crit_section_begin_time[thread_id];

    lock_hold_time[thread_id] += time_spent_in_critical_section;
    lock->total_lock_hold_time += time_spent_in_critical_section;
}
