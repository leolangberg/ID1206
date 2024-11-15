#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdatomic.h>

// To mitigate the impact of busy waiting. Tom Anderson suggested to add
// a delay after each attempt to acquire a lock. This delay may be
// incremented in each iteration as shown in the following pseudo-code.
// --------------------------------------------------------------------------
//  type lock = (unlocked, locked)
//
//  procedure acquire_lock (L : ^lock)
//      delay : integer := 1
//      while test_and_set (L) = locked     // returns old value
//          pause(delay)                    // consume this many units of time
//          delay := delay * 2
//
//  procedure release_lock (L : ^lock)
//      lock^ := unlocked
// --------------------------------------------------------------------------
// Update the code implemented in the previous task by adding a delay using
// the POSIX function usleep(). Check whether performance differences
// can be observed.

int counter = 0;
const int nthr = 1000; // for some reason threads cap at 2^11 = 2048;

typedef struct {
        /* members to be defined */
        atomic_flag lock;
} mysemaphore_t;

mysemaphore_t sem;

int sem_init(mysemaphore_t *s) {
        /* Initialize flag by clearing it. */
        atomic_flag_clear(&s->lock);
        return 0;
}

int sem_wait(mysemaphore_t *s) {
        /* Continously check if lock has become available. */
        int delay = 1;
        while(atomic_flag_test_and_set(&s->lock)) {
                // Mitigate busy waiting.
                usleep(delay);
                delay = delay * 2;
        }
        return 0;
}

int sem_post(mysemaphore_t *s) {
        /* Unlock semaphore. */
        atomic_flag_clear(&s->lock);
        return 0;
}

void *func() {
        sleep(1);
        sem_wait(&sem);
        counter++;         // Critical section
        sem_post(&sem);
    
}

int main() {
        pthread_t thr[nthr];
        sem_init(&sem);

        for(int i = 0; i < nthr; i++) {
                pthread_create(&thr[i], NULL, &func, NULL);
        }

        for(int i = 0; i < nthr; i++) {
                pthread_join(thr[i], NULL);
        }

        printf("counter = %d\n", counter);
        return 0;

        // Kolla performance difference med task2.
}