# RAID Simulation in Go

## Overview

* Simulates different RAID levels using Go
* Uses 5 files (`disk0.dat` to `disk4.dat`) to represent physical disks
* Supports:
  * RAID 0 (Striping)
  * RAID 1 (Mirroring)
  * RAID 4 (Dedicated Parity)
  * RAID 5 (Distributed Parity using XOR)
* Includes benchmarking for:
  * Write performance
  * Read performance
* Uses `fsync` to simulate real disk behavior

## Program Design

### `disk.go`
* Simulates a physical disk using a file
* Responsibilities:
  * Create disk files
  * Read block from file
  * Write block to file
  * Flush writes using `Sync()` (important for realism)

### `raid0.go` (Striping)
* Distributes data across all disks
* Logic:
  * `disk = blockNum % NumDisks`
  * `block = blockNum / NumDisks`
* No redundancy → best performance

### `raid1.go` (Mirroring)
* Writes same data to all disks
* Logic:
  * Write → all disks
  * Read → from first disk
* High redundancy, poor write performance

### `raid4.go` (Dedicated Parity)
* Last disk stores parity
* Logic:
  * Data written to one disk
  * Parity updated using XOR
* Write involves:
  * Read old data
  * Read old parity
  * Write new data
  * Write new parity

### `raid5.go` (Distributed Parity)
* Parity rotates across disks
* Logic:
  * Parity disk = `stripe % NumDisks`
  * Avoids single parity bottleneck
* Similar XOR-based parity update as RAID 4

### `main.go`
* Initializes disks
* Runs benchmark for each RAID level
* Measures:
  * Total write time
  * Total read time
  * Per-block latency

## Instructions to Compile and Run

### Step 1: Initialize Go module (first time only)  
go mod init raid-sim //Initialize Go module (first time only)

### Step 2: Run the program  
go run .

## Cleanup Before Re-running

* Delete old disk files:
rm disk*.dat

## Library Requirements / Dependencies

* Go standard library only:
  * `os` → file operations
  * `fmt` → printing output
  * `time` → benchmarking

## Results:

====== RAID 0 ======  
Total Blocks: 25000  
Write Time: 1m23.6523736s  
Read Time: 385.7688ms  
Write per block: 3.346094ms  
Read per block: 15.43µs  

====== RAID 1 ======  
Total Blocks: 25000  
Write Time: 7m43.0206944s  
Read Time: 615.5205ms  
Write per block: 18.520827ms  
Read per block: 24.62µs  

====== RAID 4 ======  
Total Blocks: 25000  
Write Time: 3m7.0978942s  
Read Time: 538.8766ms  
Write per block: 7.483915ms  
Read per block: 21.555µs  
  
====== RAID 5 ======  
Total Blocks: 25000  
Write Time: 2m59.0262188s  
Read Time: 508.7254ms  
Write per block: 7.161048ms  
Read per block: 20.349µs  

## Observations

* RAID 0:
  * Fastest read and write
  * No redundancy overhead

* RAID 1:
  * Slowest write (writes to all disks)
  * Read performance reasonable

* RAID 4:
  * Slower writes due to parity updates
  * Single parity disk bottleneck

* RAID 5:
  * Better than RAID 4
  * Distributed parity reduces bottleneck

## Analysis

* Results follow expected RAID behavior:

  * Write Performance:
    * RAID 0 > RAID 5 > RAID 4 > RAID 1
  * Read Performance:
    * RAID 0 > RAID 5 ≈ RAID 4 > RAID 1

-----------------------------






