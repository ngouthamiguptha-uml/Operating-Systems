package main

import (
	"HW2/locks"
	"fmt"
	"sync"
	"time"
)

const (
	iterations = 10000
)

// benchmarkCAS runs CASLock under contention.
// numThreads = number of goroutines competing for the same lock.
func benchmarkCAS(numThreads int) {
	// One shared lock instance for all goroutines.
	var lock locks.CASLock
	lock.Init()

	var wg sync.WaitGroup
	wg.Add(numThreads)

	// Start overall timer.
	start := time.Now()

	// totalWait accumulates total waiting time (nanoseconds) across all lock acquisitions.
	var totalWait int64

	for i := 0; i < numThreads; i++ {
		go func() {
			defer wg.Done()

			for j := 0; j < iterations; j++ {
				// Measure time spent waiting to acquire the lock.
				t1 := time.Now()
				lock.Lock()
				wait := time.Since(t1)

				// Adding all wait time across goroutines.
				totalWait += wait.Nanoseconds()

				// critical section
				// To simulate a small amount of work while holding the lock.
				time.Sleep(time.Microsecond)

				lock.Unlock()
			}
		}()
	}

	// Wait for all goroutines to finish.
	wg.Wait()

	elapsed := time.Since(start)

	fmt.Printf("CASLock Threads=%d AvgWait=%d ns TotalTime=%v\n",
		numThreads,
		totalWait/int64(numThreads*iterations),
		elapsed)
}

// benchmarkTicket runs TicketLock under contention.
// Same structure as benchmarkCAS, but uses TicketLock.
func benchmarkTicket(numThreads int) {
	var lock locks.TicketLock
	lock.Init()

	var wg sync.WaitGroup
	wg.Add(numThreads)

	start := time.Now()
	var totalWait int64

	for i := 0; i < numThreads; i++ {
		go func() {
			defer wg.Done()

			for j := 0; j < iterations; j++ {
				t1 := time.Now()
				lock.Lock()
				wait := time.Since(t1)

				totalWait += wait.Nanoseconds()

				// Critical section work
				time.Sleep(time.Microsecond)

				lock.Unlock()
			}
		}()
	}

	wg.Wait()
	elapsed := time.Since(start)

	fmt.Printf("TicketLock Threads=%d AvgWait=%d ns TotalTime=%v\n",
		numThreads,
		totalWait/int64(numThreads*iterations),
		elapsed)
}

func main() {
	// goroutine Thread levels to test.
	threadLevels := []int{2, 4, 8, 16, 32}

	for _, t := range threadLevels {
		benchmarkCAS(t)
		benchmarkTicket(t)
		fmt.Println()
	}
}
