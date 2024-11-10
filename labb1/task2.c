#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// #define N 524288*2*2*2*2*2*2*2
// #define NTHRD // num of threads.
// #define M 10

int N = 2048;
int M = 100;

// Compare run-time of program with 1 thread compared to 2 threads.
typedef struct { int imin; int imax; double psum; } args_t;
double *a;


void *partial_sum(void *p) {
    args_t *args = (args_t *) p;
    args->psum = 0.0;
    for(int i = args->imin; i < args->imax; i++) {
        args->psum += a[i];
    }
    return NULL;
}

void operation(double *time_array, int NTHRD) {

    // Time measurement
    struct timespec start, stop;
    double time;

    
    pthread_t thrd[NTHRD];
    args_t thrd_args[NTHRD];

    // Create input array
    a = (double *) malloc(N * sizeof(double));
    for(int i = 0; i < N; i++) {
        a[i] = i;
    }

    // Start time
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start);

     // Create threads
    for(int i = 0; i < NTHRD; i++) {
        thrd_args[i].imin = i * (N / NTHRD);
        thrd_args[i].imax = (i + 1) * (N / NTHRD);
        if(pthread_create(&thrd[i], NULL, partial_sum, &thrd_args[i])) {
            perror("pthread_create");
            _exit(1);
        }
    }

    // Wait for threads to finish
    for(int i = 0; i < NTHRD; i++) {
        if(pthread_join(thrd[i], NULL)) {
            perror("pthread_join");
            _exit(1);
        }
    }

    // Compute global sum
    double gsum = 0.0;
    for(int i = 0; i < NTHRD; i++) {
        gsum += thrd_args[i].psum;
    }

    //printf("gsum = %.1f\n", gsum);

    // Stop time
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &stop);
    time = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_nsec - start.tv_nsec) / 1e6;
    *time_array = time;
    
}

int compare(const void *a, const void *b) {
    // Cast the pointers to (double *) and compare the values
    double diff = *(double *)a - *(double *)b;
    if (diff < 0) return -1;  // a < b
    if (diff > 0) return 1;   // a > b
    return 0;                 // a == b
}

int main(void) {

   for (int maxDoubles = 0; maxDoubles < 10; maxDoubles++) {

        N = N * 2;

        double time_array[M];
        double time_array2[M];

        
        for(int i = 0; i < M; i++) {
            operation(&time_array[i], 1);
            operation(&time_array2[i], 2);
        }
        

        // Array of doubles to sort
        int size = sizeof(time_array) / sizeof(time_array[0]);
        int size2 = sizeof(time_array2) / sizeof(time_array2[0]);

        // Sort the array using qsort
        qsort(time_array, size, sizeof(double), compare);
        qsort(time_array2, size2, sizeof(double), compare);

        printf("Elapsed time for %d repetitions with %d threads, array length %d, %.6f milliseconds\n", M, 1, N, time_array[M / 2]);
        printf("Elapsed time for %d repetitions with %d threads, array length %d, %.6f milliseconds\n", M, 2, N, time_array2[M / 2]);

   }


    return 0;
}

