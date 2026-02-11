#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>

#define NUM_COUNT 1000000   // Increased the number to benchmark the performance of threads

int buffer;                // Shared buffer (one-slot)
int buffer_full = 0;       // 0 = empty, 1 = full

pthread_mutex_t mutex;     // Mutex to protect shared data
pthread_cond_t cond;       // Condition variable for synchronization

void* producer(void* arg) {
    for (int i = 1; i <= NUM_COUNT; i++) {
        pthread_mutex_lock(&mutex);

        // Wait until buffer becomes empty
        while (buffer_full) {
            pthread_cond_wait(&cond, &mutex);
        }

        buffer = i;        // Produce data
        buffer_full = 1;

        pthread_cond_signal(&cond); // Wake up consumer
        pthread_mutex_unlock(&mutex);
    }

    return 0;
}

void* consumer(void* arg) {
    for (int i = 1; i <= NUM_COUNT; i++) {
        pthread_mutex_lock(&mutex);

        // Wait until buffer becomes full
        while (!buffer_full) {
            pthread_cond_wait(&cond, &mutex);
        }

        buffer_full = 0;   // Consume data

        pthread_cond_signal(&cond); // Wake up producer
        pthread_mutex_unlock(&mutex);
    }
    
    return 0;
}

int main() {
    pthread_t prod, cons;
    struct timespec start, end;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Start timing
    clock_gettime(CLOCK_MONOTONIC, &start);

    assert(pthread_create(&prod, NULL, producer, NULL) == 0);
    assert(pthread_create(&cons, NULL, consumer, NULL) == 0);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    // End timing
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("Thread-based execution time: %f seconds\n", elapsed);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}