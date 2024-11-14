#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdatomic.h>

// Implement your own binary semaphore lock using atomic functions
// like atomic_flag_test_and_set() to make the following program run.

int counter = 0;
const int nthr = 1000;

typedef struct {
        /* members to be defined */
} mysemaphore_t;

mysemaphore_t sem;

int sem_init(mysemaphore_t *s) {
        /* function to be implemented */
        return 0;
}

int sem_wait(mysemaphore_t *s) {
        /* function to be implemented */
        return 0;
}

int sem_post(mysemaphore_t *s) {
        /* function to be implemented */
        return 0;
}

void *func() {
        sleep(1);
        sem_wait(&sem);
        counter++;
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
}
