#ifndef TLB_H
#define TLB_H

#include <vector>
#include <iostream>

/*
TLB Entry structure
Stores mapping between virtual page number and physical frame
*/
struct TLBEntry {
    int vpn;            // Virtual Page Number
    int pfn;            // Physical Frame Number
    bool valid;         // Valid bit
    int last_used;      // For LRU replacement

    TLBEntry() {
        vpn = -1;
        pfn = -1;
        valid = false;
        last_used = 0;
    }
};

class TLB {
private:
    int size;                       // Number of entries in TLB
    std::vector<TLBEntry> entries;
    int time_counter;

public:

    TLB(int s) {
        size = s;
        entries.resize(size);
        time_counter = 0;
    }

    /*
    Lookup VPN in TLB
    Return PFN if found
    Return -1 if miss
    */
    int lookup(int vpn) {

        time_counter++;

        int index = vpn % size;   // Direct mapped

        if (entries[index].valid && entries[index].vpn == vpn) {
            entries[index].last_used = time_counter;
            return entries[index].pfn;     // TLB HIT
        }

        return -1;  // TLB MISS
    }

    /*
    Insert new mapping into TLB
    */
    void insert(int vpn, int pfn) {

        int index = vpn % size;  // Direct mapped position

        entries[index].vpn = vpn;
        entries[index].pfn = pfn;
        entries[index].valid = true;
        entries[index].last_used = time_counter;
    }

};

#endif