## Overview:

This project has Design and Implementation of 2 locks along with a benchmark to evaluate the waiting time of each lock, under different contention. This program was developed and tested using: Go 1.20+ version

- Compare-And-Swap (CAS) Spin Lock
- Ticket Lock

Both locks are implemented using Go’s sync/atomic package.  
A benchmarking method is included to evaluate average waiting time under different contention levels.

## File Description

### caslock.go
Contains implementation of CAS-based spin lock.

### ticketlock.go
Contains implementation of FIFO Ticket lock.

### main.go
Contains benchmarking logic and performance evaluation logic.

## Design of the Program

### Compare-And-Swap (CAS) Spin Lock

1. Structure which contains flag.
flag = 0 -> lock is free
flag = 1 -> lock is held

2. Lock Acquire logic  
The lock repeatedly attempts: atomic.CompareAndSwapInt32(&flag, 0, 1)  
If the CAS operation fails, the goroutine spins until it succeeds.

3. Unlock
The lock is released using: atomic.StoreInt32(&flag, 0)

### Ticket Lock

1. Structure has ticket and turn.  
ticket -> next ticket number to assign  
turn -> currently serving ticket  

2. Lock Acquire logic
Each goroutine obtains a ticket number using: atomic.AddInt32(&ticket, 1) - 1  
This behaves like Fetch-And-Add.  

Then it spins until: turn == myTicket  

3. Unlock
The lock increments turn using: atomic.AddInt32(&turn, 1)

### Benchmark Design

The benchmark is implemented in main.go.

### Methodology

- Launch multiple goroutines.
- Each goroutine performs a fixed number of lock/unlock iterations.
- Measure waiting time for each lock acquisition.
- Compute average waiting time.

### Contention Levels Tested

The program evaluates performance under:
- 2 threads
- 4 threads
- 8 threads
- 16 threads
- 32 threads

The same workload is applied to both locks under each configuration to ensure fairness.

## Instructions to compile and run

go run main.go

The program will automatically:

- Run benchmarks for both locks
- Test multiple contention levels
- Print average waiting time results to console

## Dependencies and Libraries Used

This project uses only Go’s standard library.

Packages Used

- sync
- sync/atomic
- runtime
- time
- fmt

## Output

CASLock Threads=2 AvgWait=566027 ns TotalTime=11.3405942s  
TicketLock Threads=2 AvgWait=582301 ns TotalTime=11.6454164s  

CASLock Threads=4 AvgWait=1736047 ns TotalTime=23.2605847s  
TicketLock Threads=4 AvgWait=1774146 ns TotalTime=23.6079118s  

CASLock Threads=8 AvgWait=7861434 ns TotalTime=1m30.6769733s  
TicketLock Threads=8 AvgWait=6407419 ns TotalTime=1m13.195985s  

CASLock Threads=16 AvgWait=35659472 ns TotalTime=6m23.7141147s  
TicketLock Threads=16 AvgWait=37990081 ns TotalTime=6m45.231464s  

CASLock Threads=32 AvgWait=47059745 ns TotalTime=8m11.6634466s  
TicketLock Threads=32 AvgWait=53885880 ns TotalTime=9m16.0524037s  

Where:  
Threads = number of goroutines contending on the lock  
AvgWait = average time spent waiting to acquire the lock (nanoseconds)  
TotalTime = total time for all goroutines to finish all iterations  

## Analysis

Performance of CAS Lock and Ticket Lock is analysed using different numbers of threads. The goal was to measure the average waiting time before entering the critical section.  

1. Low Contention (2–4 Threads)  

Results:  
CAS and Ticket locks perform almost the same.  
CAS is slightly faster.  

Under low contention, CAS lock performs slightly better.  

2. Moderate Contention (8 Threads)

Results:  
Ticket lock performs better than CAS.  

At this level, many threads try to acquire the lock at the same time.  

In CAS lock:  
All threads repeatedly try to update the same variable.  
This creates heavy CPU and cache traffic.  

In Ticket lock:  
Each thread waits for its turn.  
Only one atomic update happens during unlock.  

This reduces unnecessary contention. Therefore, Ticket lock scales better at moderate contention.  

3. High Contention (16–32 Threads)

Results:  
Waiting time becomes very large for both locks.  
CAS becomes slightly faster than Ticket lock again.  
Total execution time increases significantly in minutes.  

At very high contention: A lot of threads are competing. Therefore, CPU scheduling overhead increases. Context switching also increases.  

Ticket lock enforces strict order (FIFO). If the next thread in line is delayed or preempted, everyone else must wait.  

CAS lock does not enforce strict order. Whichever thread runs first may acquire the lock. This sometimes may improve overall throughput.  

At extreme contention, fairness can slightly reduce performance.  

