#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <time.h>

#define NUM_COUNT 1000000

int main() {
    int pipefd[2]; // pipefd[0] = read end, pipefd[1] = write end

    // Create a pipe
    int r = pipe(pipefd);
    assert(r == 0);

    struct timespec start, end;

    // Start timing before fork
    clock_gettime(CLOCK_MONOTONIC, &start);

    pid_t p1 = fork();
    assert(p1 >= 0);

    if (p1 > 0) {
        // Parent process -> Producer
        close(pipefd[0]); // Close unused read end

        for (int i = 0; i < NUM_COUNT; i++) {
            ssize_t nw = write(pipefd[1], &i, sizeof(i));
            assert(nw == (ssize_t) sizeof(i));
        }

        close(pipefd[1]); // Finished writing
        wait(NULL);       // Wait for consumer

        // Stop timing after consumer finishes
        clock_gettime(CLOCK_MONOTONIC, &end);

        double time_taken =
            (end.tv_sec - start.tv_sec) +
            (end.tv_nsec - start.tv_nsec) / 1e9;

        printf("IPC (process + pipe) time: %f seconds\n", time_taken);

        exit(EXIT_SUCCESS);

    } else {
        // Child process -> Consumer
        close(pipefd[1]); // Close unused write end

        int num;
        for (int i = 0; i < NUM_COUNT; i++) {
            ssize_t nr = read(pipefd[0], &num, sizeof(num));
            assert(nr == (ssize_t) sizeof(num));
        }

        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }

    return 0;
}
