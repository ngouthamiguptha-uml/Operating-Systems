# TLB Simulator

## Overview
This project simulates the behavior of a Translation Lookaside Buffer (TLB) in a virtual memory system. It models how virtual addresses are translated into physical addresses using a TLB and a single-level page table. The simulator measures TLB performance in terms of hit and miss rates under different configurations.

## Program Design

The project consists of the following components:

### 1. TLB (TLB.h)
- Implements a **direct-mapped TLB**
- Stores mappings of:
  - Virtual Page Number (VPN)
  - Physical Frame Number (PFN)
- Supports:
  - `lookup(vpn)` → returns PFN or -1 (miss)
  - `insert(vpn, pfn)` → updates TLB entry

### 2. Page Table (PageTable.h)
- Implements a **single-level page table**
- Uses a vector where:
  - Index = VPN
  - Value = PFN
- Uses **identity mapping**

### 3. Simulator (Simulator.h)
- Core component that:
- Processes memory access trace
- Extracts VPN from address
- Performs TLB lookup
- Handles TLB miss using page table
- Updates TLB
- Tracks:
- TLB hits
- TLB misses
- Hit rate

### 4. Workload Generator (WorkloadGenerator.h)
- Generates **synthetic memory access traces**
- Produces random addresses within a given range

### 5. Main Program
- Configures simulation parameters
- Generates workload
- Runs simulation for different TLB sizes (16 and 64)
- Prints performance statistics

## Steps to Compile

Use a C++ compiler like `g++`:  
g++ main.cpp -o tlb_sim
./tlb_sim

## Libraries Used

<vector> → dynamic arrays  
<iostream> → input/output  
<cstdlib> → random number generation (rand())  

## Results

The program outputs:

- Total memory accesses
- TLB hits
- TLB misses
- TLB hit rate

===== TLB Size 16 =====  
Total Accesses: 100000  
TLB Hits: 99992  
TLB Misses: 8  
TLB Hit Rate: 0.99992  

===== TLB Size 64 =====  
Total Accesses: 100000  
TLB Hits: 99992  
TLB Misses: 8  
TLB Hit Rate: 0.99992  

## Observations

- Increasing TLB size improves hit rate.
- When TLB size ≥ number of pages:  
    Almost all accesses become hits after initial misses.
-Direct-mapped TLB may suffer from:  
    Conflict misses (multiple VPNs mapping to same index).

## Future Improvements:

- Implement set-associative TLB
- Add 3 replacement policy
- Simulate page faults
- Extend to multi-level page tables

--------
