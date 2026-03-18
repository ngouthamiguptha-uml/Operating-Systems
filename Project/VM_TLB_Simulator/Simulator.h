#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "TLB.h"
#include "PageTable.h"
#include <vector>
#include <iostream>

class Simulator {

private:

    int page_size;

    TLB tlb;
    PageTable page_table;

    int tlb_hits;
    int tlb_misses;

public:

    Simulator(int tlb_size, int num_pages, int psize)
        : tlb(tlb_size), page_table(num_pages)
    {
        page_size = psize;
        tlb_hits = 0;
        tlb_misses = 0;
    }

    /*
    Simulate memory accesses
    */
    void run(std::vector<int> &trace) {

        for (int addr : trace) {

            int vpn = addr / page_size;

            int pfn = tlb.lookup(vpn);

            if (pfn != -1) {
                tlb_hits++;
            }
            else {

                tlb_misses++;

                // Page table lookup
                pfn = page_table.getFrame(vpn);

                // Update TLB
                tlb.insert(vpn, pfn);
            }
        }
    }

    void printStats() {

        int total = tlb_hits + tlb_misses;

        double hit_rate = (double)tlb_hits / total;

        std::cout << "Total Accesses: " << total << std::endl;
        std::cout << "TLB Hits: " << tlb_hits << std::endl;
        std::cout << "TLB Misses: " << tlb_misses << std::endl;
        std::cout << "TLB Hit Rate: " << hit_rate << std::endl;
    }
};

#endif