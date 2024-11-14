#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdatomic.h>

const int r      = 1;
const int square = 4*r*r;
const int nthr   = 25;
int ntry = 0;
int nhit = 0;

pthread_mutex_t lock;
const int periods = 10;
int stop = 0;

// Montecarlo operation. Update number of tries
// and place a random (x,y) point on square.
// If inside circle then it is a hit.   
void *montecarlo(void *) {
    while(!stop) {
        srand48(time(NULL) + (long) pthread_self());
        pthread_mutex_lock(&lock);
        ntry++;
        pthread_mutex_unlock(&lock);

        double x = drand48();
        double y = drand48();

        if(((x*x) + (y*y)) <= (r*r)) {
                pthread_mutex_lock(&lock);
                nhit++;
                pthread_mutex_unlock(&lock);
        }
    }
}

// Create threads that all perform a hit or miss.
// Finally calculate quotient of nhit / ntry * square
// to get approximate area of circle.
int main(void) {

    pthread_t thr[nthr];
    pthread_mutex_init(&lock, NULL);
    double circle;

    for(int i = 0; i < nthr; i++) {
        pthread_create(&thr[i], NULL, montecarlo, NULL);
    }

    for(int i = 0; i < periods; i++) {
        sleep(1);
        pthread_mutex_lock(&lock);

        circle = (double) ((double) nhit / (double) ntry) * (double) square;
        printf("Total nhit: %f, ntry: %f, nhit/ntry: %f\n", (double) nhit, (double) ntry, (double) nhit / (double) ntry);
        printf("Montecarlo Circle: %f\n", circle);

        pthread_mutex_unlock(&lock);
    }
    stop = 1;
    sleep(1);

    for(int i = 0; i < nthr; i++) {
        pthread_join(thr[i], NULL);
    }

    circle = (double) ((double) nhit / (double) ntry) * (double) square;
    printf("Total nhit: %f, ntry: %f, nhit/ntry: %f\n", (double) nhit, (double) ntry, (double) nhit / (double) ntry);
    printf("Montecarlo Circle: %f\n", circle);

    return 0;
}                     


