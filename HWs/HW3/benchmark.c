#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "list.h"

#define KEY_RANGE 1000

// Shared list pointer (actual structure is defined in both cll_global and cll_hoh files)
static list_t *shared_list;

// Benchmark parameters (shared by all threads)
static int num_threads;
static int operations;
static int update_ratio;


// Thread Function
void *worker(void *arg) {

    // Per-thread random seed (safe for coursework)
    // Combines current time and thread ID
    unsigned int seed = time(NULL) ^ (unsigned int)pthread_self();

    for (int i = 0; i < operations; i++) {

        // Generate random key
        int key = rand_r(&seed) % KEY_RANGE;

        // Decide operation type (0–99)
        int op = rand_r(&seed) % 100;

        if (op < update_ratio) {
            // Update operation:
            // 50% insert, 50% delete
            if (rand_r(&seed) % 2)
                list_insert(shared_list, key);
            else
                list_delete(shared_list, key);
        } else {
            // Read operation
            list_lookup(shared_list, key);
        }
    }

    return NULL;
}


//Main function
int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Usage: %s <threads> <operations> <update_ratio>\n", argv[0]);
        return 1;
    }

    // Read command-line arguments
    num_threads  = atoi(argv[1]);
    operations   = atoi(argv[2]);
    update_ratio = atoi(argv[3]);

    pthread_t threads[num_threads];

    // Create shared list
    shared_list = list_create();
    if (!shared_list) {
        printf("Error: list_create failed\n");
        return 1;
    }

    // Start timing
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create worker threads
    for (int i = 0; i < num_threads; i++)
        pthread_create(&threads[i], NULL, worker, NULL);

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    // Stop timing
    clock_gettime(CLOCK_MONOTONIC, &end);

    double time_sec = (end.tv_sec - start.tv_sec) +
                      (end.tv_nsec - start.tv_nsec) / 1e9;

    long total_ops = (long)num_threads * operations;

    printf("Threads: %d\n", num_threads);
    printf("Total operations: %ld\n", total_ops);
    printf("Update ratio: %d%%\n", update_ratio);
    printf("Execution time: %.4f seconds\n", time_sec);
    printf("Throughput: %.2f ops/sec\n", total_ops / time_sec);

    // Clean up memory
    list_destroy(shared_list);

    return 0;
}