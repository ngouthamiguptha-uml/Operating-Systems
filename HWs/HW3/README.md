## 2. Comparing Queue algorithms:

## Overview

This project implements and benchmarks two concurrent sorted linked list designs using POSIX threads:
- Global Lock List (cll_global.c)
A single mutex protects the entire list.

- Hand-over-Hand Locking List (cll_hoh.c)
Each node has its own mutex.

A benchmarking program (benchmark.c) evaluates performance of both under varying thread counts and update workloads.

## Program design

### File Description
list.h	- Header file containing the public API  
cll_global.c - Global lock implementation  
cll_hoh.c - Hand-over-hand locking implementation  
benchmark.c	- Multi-threaded benchmark driver  

Both cll_global.c and cll_hoh.c support the following operations:  
list_insert() – Insert key in sorted order  
list_delete() – Delete key if present  
list_lookup() – Search for a key  
list_create() – Initialize list  
list_destroy() – Free list and resources  

list.h defines an opaque type: typedef struct list list_t;  
Both implementations define struct list internally but has same API like mentioned above.

### cll_global.c
Every operation (insert, delete, lookup) locks the entire list.
Only one thread pthread_mutex_t can access the list at a time.

### cll_hoh.c
Each node contains its own mutex.  
Traversal locks the next node before unlocking the previous node.  
Multiple threads can operate on different parts of the list simultaneously.  

### Benchmark file (benchmark.c)
The benchmark measures:

(i) Execution time  
(ii) Throughput (operations per second)    

Benchmark Design:  

1. Shared list : static list_t *shared_list;
Points to a single shared linked list.  
All threads operate on this same list.  
The internal synchronization depends on the linked list implementation.  

2. Global Parameters like - static int num_threads; static int operations; static int update_ratio;
These are initialized from command-line arguments and shared across all threads. 
<threads> <operations_per_thread> <update_ratio>  
num_threads → Number of concurrent worker threads  
operations → Number of operations per thread  
update_ratio → Percentage of update operations (0–100)  

3. Thread Execution
Each thread executes the worker() function.
Each worker:
- Runs operations iterations
- Performs random operations
- Uses a thread-local random seed (rand_r) for thread safety
This avoids data races on random number generation.

4. Workload Generation Logic:
For each iteration:

- Generate a random key: key = rand_r(&seed) % KEY_RANGE;
- Decide operation type: op = rand_r(&seed) % 100;
- Operation selection logic:
If op < update_ratio: Perform update of 50% insert, 50% delete  
Else: Perform lookup  
  
This design allows precise control of workload mix.  

## Instructions to compile and run

1. Compile Global Lock Version  
gcc -pthread benchmark.c cll_global.c -o benchmark_global  

2. Compile Hand-Over-Hand Version  
gcc -pthread benchmark.c cll_hoh.c -o benchmark_hoh  

To run: ./executable <threads> <operations_per_thread> <update_ratio>  

## Requirements / Dependencies

Standard C libraries:  

- <stdio.h>  
- <stdlib.h>  
- <time.h>  

POSIX Threads library:  

- <pthread.h>  

## Output/Results along with observation:

1. ./benchmark_global 2 10000 20 -> Low Threads, Low Workload  
Threads: 2  
Total operations: 20000  
Update ratio: 20%  
Execution time: 0.0625 seconds  
Throughput: 320013.15 ops/sec  

With only 2 threads and 20% updates, the global lock performs extremely well.  
Since contention is low and most operations are lookups, threads rarely block each other.  
The overhead of acquiring a single mutex is minimal.  

2. ./benchmark_global 4 500000 50 -> Moderate Threads, Heavy Workload  
Threads: 4  
Total operations: 2000000  
Update ratio: 50%  
Execution time: 275.4075 seconds  
Throughput: 7261.97 ops/sec  

Performance drops dramatically. Throughput decreases from 320k to 7k. Reasons are - 
4 threads now compete for one global mutex.  
50% updates mean frequent structural modifications.  
Only one thread can access the list at a time.  

3. ./benchmark_global 8 1000000 80 -> High Threads, Very High Updates    
Threads: 8  
Total operations: 8000000  
Update ratio: 80%  
Execution time: 3711.4236 seconds  
Throughput: 2155.51 ops/sec  

8 threads fighting for one mutex.  
80% updates (inserts + deletes).  
Every operation requires full list lock.  
Heavy blocking and context switching.  
Therefore, global lock does not scale well with threads or update-heavy workloads.  

4. ./benchmark_hoh 2 10000 20 -> Low Threads, Low Workload   
Threads: 2
Total operations: 20000
Update ratio: 20%
Execution time: 0.2842 seconds
Throughput: 70367.63 ops/sec

Compared to global lock (320k ops/sec), HOH is slower in this small case. Because, HOH uses multiple mutexes (per-node locks).
More lock/unlock overhead.  

5. ./benchmark_hoh 4 50000 50 -> Moderate threads, moderate workload  
Threads: 4
Total operations: 200000
Update ratio: 50%
Execution time: 9.6642 seconds
Throughput: 20695.04 ops/sec

HOH maintains reasonable throughput (~20k ops/sec) whereas Global collapses under contention (~7k ops/sec).  

6. ./benchmark_hoh 4 500000 20 --> Very Large Workload  

Takes very long time to execute. Because With many inserts, list grows large. Each operation requires locking node-by-node traversal. Lock overhead increases with list length.

## Analysis:

1. For small workloads → Global Lock is faster because of Lower overhead, Minimal contention & Simpler locking.
2. Increasing threads → Global Lock collapses because of High blocking, Poor scalability.
3. HOH scales better as it allows parallel access to different nodes and reduces contention scope. Also, maintains a stable throughput under moderate load.
4. HOH provides better parallelism but introduces higher complexity and more locking overhead.
5. However, Heavy workloads affects both as traversal cost grows with list size.

Also, another mutex at list level is added to protect head pointer because concurrent inserts or deletes at the beginning of the list caused race conditions or segmentation faults when multiple threads attempted to read or modify the head simultaneously.

-----------------------------------------




