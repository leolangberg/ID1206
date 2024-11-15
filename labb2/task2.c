#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdatomic.h>

// Implement your own binary semaphore lock using atomic functions
// like atomic_flag_test_and_set() to make the following program run.

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
        while(atomic_flag_test_and_set(&s->lock)) {
                // busy wait.
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
        //printf("Thread %ld entering critical section\n", (long) pthread_self());
        counter++;         // Critical section
        //printf("Thread %ld leaving  critical section\n", (long) pthread_self());
        sem_post(&sem);
    
}

int main() {
        pthread_t thr[nthr];
        pthread_attr_t attr;
        size_t stack_size = nthr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, stack_size);
        sem_init(&sem);

        for(int i = 0; i < nthr; i++) {
                pthread_create(&thr[i], &attr, &func, NULL);
        }

        for(int i = 0; i < nthr; i++) {
                pthread_join(thr[i], NULL);
        }

        printf("counter = %d\n", counter);
        return 0;
}