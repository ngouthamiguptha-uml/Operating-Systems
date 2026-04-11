package main

import (
	"fmt"
	"time"
)

// Common interface (as required)
type RAID interface {
	Write(blockNum int, data []byte)
	Read(blockNum int) []byte
}

// Create 5 disk files
func initDisks() []*Disk {
	disks := make([]*Disk, NumDisks)
	for i := 0; i < NumDisks; i++ {
		name := fmt.Sprintf("disk%d.dat", i)
		disks[i] = NewDisk(name)
	}
	return disks
}

// Benchmark function
func runBenchmark(name string, raid RAID, totalBlocks int) {

	data := make([]byte, BlockSize)

	fmt.Println("======", name, "======")

	// WRITE TEST
	start := time.Now()
	for i := 0; i < totalBlocks; i++ {
		raid.Write(i, data)
	}
	writeTime := time.Since(start)

	// READ TEST
	start = time.Now()
	for i := 0; i < totalBlocks; i++ {
		raid.Read(i)
	}
	readTime := time.Since(start)

	fmt.Println("Total Blocks:", totalBlocks)
	fmt.Println("Write Time:", writeTime)
	fmt.Println("Read Time:", readTime)
	fmt.Println("Write per block:", writeTime/time.Duration(totalBlocks))
	fmt.Println("Read per block:", readTime/time.Duration(totalBlocks))
	fmt.Println()
}

func main() {

	totalBlocks := 25000 // ~100MB

	runBenchmark("RAID 0", &RAID0{initDisks()}, totalBlocks)
	runBenchmark("RAID 1", &RAID1{initDisks()}, totalBlocks)
	runBenchmark("RAID 4", &RAID4{initDisks()}, totalBlocks)
	runBenchmark("RAID 5", &RAID5{initDisks()}, totalBlocks)
}
