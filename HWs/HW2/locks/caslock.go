package locks

import (
	"runtime"
	"sync/atomic"
)

// CASLock is a simple spin lock implemented using atomic Compare-And-Swap (CAS).
// flag : 0 = unlocked, 1 = locked
type CASLock struct {
	flag int32
}

// Initinitializes the lock to "unlocked".
func (l *CASLock) Init() {
	atomic.StoreInt32(&l.flag, 0)
}

// Lock spins until it can atomically change flag from 0 -> 1.
// CompareAndSwapInt32(&flag, 0, 1):
// - returns true if it successfully changed 0 to 1 (lock acquired)
// - returns false if someone else holds the lock
func (l *CASLock) Lock() {
	for !atomic.CompareAndSwapInt32(&l.flag, 0, 1) {
		// Yield execution so other goroutines can run.
		// This reduces aggressive CPU spinning under contention.
		runtime.Gosched()
	}
}

// Unlock releases the lock by setting flag back to 0.
func (l *CASLock) Unlock() {
	atomic.StoreInt32(&l.flag, 0)
}
