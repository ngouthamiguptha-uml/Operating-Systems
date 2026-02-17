package locks

import (
	"runtime"
	"sync/atomic"
)

// TicketLock is a fair FIFO spin lock.
// - ticket: next ticket number to assign
// - turn:   ticket number currently allowed to enter the critical section
type TicketLock struct {
	ticket int32
	turn   int32
}

// Init resets both counters.
// With both at 0, the next goroutine gets ticket 0 and can enter immediately.
func (l *TicketLock) Init() {
	atomic.StoreInt32(&l.ticket, 0)
	atomic.StoreInt32(&l.turn, 0)
}

// Lock assigns the caller a ticket, then waits until it's their turn.
// myTurn is computed as:
//
//	atomic.AddInt32(&ticket, 1) returns the incremented value
//	subtract 1 to get the caller's ticket number (0-based)
func (l *TicketLock) Lock() {
	myTurn := atomic.AddInt32(&l.ticket, 1) - 1

	for {
		// Only the goroutine whose ticket equals "turn" can proceed.
		if atomic.LoadInt32(&l.turn) == myTurn {
			return
		}

		// Yield CPU to reduce tight spinning.
		runtime.Gosched()
	}
}

// Unlock advances the turn to the next ticket number,
// allowing the next waiting goroutine to enter.
func (l *TicketLock) Unlock() {
	atomic.AddInt32(&l.turn, 1)
}
