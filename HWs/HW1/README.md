## 1. One Producer – One Consumer using Threads (Pthreads)

## Overview

This program implements one-producer, one-consumer problem using threads in C.
The producer thread generates a number and places them into a shared buffer. The consumer then takes these values and prints them.

The program demonstrates thread-based synchronization using:

- Mutexes (pthread_mutex_t)
- Condition variables (pthread_cond_t)

## Program design

Threads

1. Producer thread

- Generates integers from 1 to NUM_COUNT
- Writes each value to a shared buffer

2. Consumer thread

- Reads integers from the shared buffer
- Prints the consumed value

### Synchronization

A mutex ensures mutual exclusion while accessing shared data.
A condition variable coordinates execution between producer and consumer:

- Producer waits if the buffer is full.
- Consumer waits if the buffer is empty.

This prevents race conditions and ensures correct ordering.

Basically, the steps followed are - 

1. Producer locks the mutex.
2. If the buffer is full, producer waits on the condition variable.
3. Producer writes data and signals the consumer.
4. Consumer wakes up, reads data, and signals the producer.
5. Steps repeat until all items are produced and consumed.

## Instructions to compile and run

gcc -pthread cp_threads.c -o cp_threads
./cp_threads

## Requirements / Dependencies

Standard C libraries:

- <stdio.h>
- <stdlib.h>
- <assert.h>

POSIX Threads library:

- <pthread.h>

---------------

## 2. Benchmark the performance of using two different approaches - IPC (Pipes) and using threads:

The process-based solution uses write() and read() system calls to perform each operation, which involves kernel activity and switching overhead. However, the use of a pipe ensures internal buffers are available, which helps the producer and consumer to run with fewer synchronizations. 

Program: cp_pipes_perf.c
Steps to Compile and Run: 
-  gcc -pthread cp_pipes_perf.c -o cp_pipes_perf
- ./cp_pipes_perf

Whereas, The thread-based solution avoids IPC system calls by using shared memory approach. However, due to the single slot buffer and condition variables, there is a strict alternation between the producer and consumer. This leads to a heavy synchronization overhead due to repeated sleep and wake operations. 

Program: cp_threads_perf.c
Steps to Compile and Run: 
- gcc -pthread cp_threads_perf.c -o cp_threads_perf
- ./cp_threads_perf

Both the programs are added with #include <time.h> library and total execution time of the program is measured for consuming N items and compared between both types.

- Records the start time before threads start. 
- Records end time after threads finish
- Compute difference

## Observation:

Theoretically, threads typically performs better than processes. But in this design - process-based implementation performed better than the thread-based implementation. Poor synchronization design can increase execution time as an overhead.

### Results:

IPC (process + pipe) time: 1.012814 seconds
Thread-based execution time: 145.214767 seconds

Although threads typically outperform processes, this experiment shows that excessive synchronization using condition variables can make a thread-based solution slower than a process-based pipe implementation.

This can be improved by using a circular type of buffer.

------------------------------------


