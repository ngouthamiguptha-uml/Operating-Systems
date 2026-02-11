#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    // pthreads API
#include <assert.h>     

#define NUM_COUNT 5     // Number of items to produce and consume

int buffer;             // Shared buffer
int buffer_full = 0;    // Flag to indicate whether buffer has data (1 = full, 0 = empty)


pthread_mutex_t mutex;  // Mutex to protect shared data
pthread_cond_t cond;    // Condition variable for synchronization

// Producer thread function
void* producer(void* arg) {
    for (int i = 1; i <= NUM_COUNT; i++) {
           
        pthread_mutex_lock(&mutex); // Acquire lock before accessing shared buffer

        // Wait while buffer is full
        while (buffer_full) {
            pthread_cond_wait(&cond, &mutex);
        }
  
        buffer = i; // Produce data
        buffer_full = 1;

        printf("Producer: %d\n", i);
        fflush(stdout);

        pthread_cond_signal(&cond); // Signal consumer that data is available    
        pthread_mutex_unlock(&mutex); // Release lock
    }
    
    return 0;
}

// Consumer thread function
void* consumer(void* arg) {
    for (int i = 1; i <= NUM_COUNT; i++) {

         pthread_mutex_lock(&mutex); // Acquire lock before accessing shared buffer
       
        // Wait while buffer is empty
        while (!buffer_full) {
            pthread_cond_wait(&cond, &mutex);
        }

        // Consume data
        printf("Consumer: %d\n", buffer);
        fflush(stdout);
      
        buffer_full = 0;    // Mark buffer as empty

        pthread_cond_signal(&cond);     // Signal producer that buffer is free
        pthread_mutex_unlock(&mutex);   // Release lock
    }

    return 0;
}

int main() {
    pthread_t prod, cons;

    // Initialize synchronization primitives
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // Create producer and consumer threads
    assert(pthread_create(&prod, NULL, producer, NULL) == 0);
    assert(pthread_create(&cons, NULL, consumer, NULL) == 0);

    // Wait for threads to finish
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    // Clean up resources
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}