#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <assert.h>

int main() {
    int pipefd[2]; // pipefd[0] = read end, pipefd[1] = write end

    // Create a pipe
    int r = pipe(pipefd);
    assert(r == 0);
    
    // Fork to create a child process
    pid_t p1 = fork();
    assert(p1 >= 0);

    if (p1 > 0) {
        // Parent process -> Producer
        close(pipefd[0]); // Close unused read end

        for (int i = 1; i <= 5; i++) {
            printf("Producer: %d\n", i);
            fflush(stdout); // Ensure output appears immediately

            // Write the number to the pipe
            ssize_t nw = write(pipefd[1], &i, sizeof(i));
            assert(nw == (ssize_t) sizeof(i)); // Make sure all bytes are written
            
            // Wait for Consumer to read before producing next
            usleep(100000); // small delay (0.1s) to ensure order
        }

        close(pipefd[1]); // Close write end after finishing
        wait(NULL);       // Wait for child process to finish
        exit(0);
    } else {
        // Child process -> Consumer
        close(pipefd[1]); // Close unused write end
        int num;

        for (int i = 1; i <= 5; i++) {
            // Read number from pipe
            ssize_t nr = read(pipefd[0], &num, sizeof(num));
            assert(nr == (ssize_t) sizeof(num)); // Make sure all bytes are read
            printf("Consumer: %d\n", num);
            fflush(stdout); // Ensure output appears immediately
        }
    }

        close(pipefd[0]); // Close read end
        exit(0);
    

    return 0;
}