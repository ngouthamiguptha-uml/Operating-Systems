#ifndef TLB_H
#define TLB_H

#include <vector>
#include <cstdlib>
#include <climits>
#include <string>

// Replacement policy enumeration
enum class ReplacementPolicy {
    LRU,
    FIFO,
    RANDOM
};

// TLB Entry structure
struct TLBEntry {
    int vpn;
    int pfn;
    bool valid;
    int last_used;    // For LRU
    int insert_time;  // For FIFO

    TLBEntry() : vpn(-1), pfn(-1), valid(false), last_used(0), insert_time(0) {}
};

// One way (set) in a set-associative TLB
struct TLBSet {
    std::vector<TLBEntry> ways;
    TLBSet() {}
    TLBSet(int num_ways) : ways(num_ways) {}
};

/*
 * Set-Associative TLB with configurable:
 *   - total_entries : total number of TLB entries
 *   - associativity : 1 = direct-mapped, n = n-way, total = fully associative
 *   - policy        : LRU | FIFO | RANDOM
 */
class TLB {
private:
    int total_entries;
    int associativity;
    int num_sets;
    ReplacementPolicy policy;
    std::vector<TLBSet> sets;
    int time_counter;

    int findVictim(int set_idx) {
        TLBSet& s = sets[set_idx];
        for (int i = 0; i < associativity; i++)
            if (!s.ways[i].valid) return i;

        if (policy == ReplacementPolicy::LRU) {
            int lru_idx = 0;
            for (int i = 1; i < associativity; i++)
                if (s.ways[i].last_used < s.ways[lru_idx].last_used)
                    lru_idx = i;
            return lru_idx;
        } else if (policy == ReplacementPolicy::FIFO) {
            int fifo_idx = 0;
            for (int i = 1; i < associativity; i++)
                if (s.ways[i].insert_time < s.ways[fifo_idx].insert_time)
                    fifo_idx = i;
            return fifo_idx;
        } else { // RANDOM
            return rand() % associativity;
        }
    }

public:
    TLB(int total, int assoc, ReplacementPolicy p)
        : total_entries(total), associativity(assoc), policy(p), time_counter(0)
    {
        num_sets = total_entries / associativity;
        sets.resize(num_sets, TLBSet(associativity));
    }

    // Flush all entries (used during context switch)
    void flush() {
        for (auto& s : sets)
            for (auto& e : s.ways)
                e.valid = false;
    }

    // Lookup VPN -> PFN. Returns -1 on miss.
    int lookup(int vpn) {
        time_counter++;
        int set_idx = vpn % num_sets;
        TLBSet& s = sets[set_idx];
        for (int i = 0; i < associativity; i++) {
            if (s.ways[i].valid && s.ways[i].vpn == vpn) {
                s.ways[i].last_used = time_counter;
                return s.ways[i].pfn;
            }
        }
        return -1; // miss
    }

    // Insert (vpn, pfn) using configured replacement policy
    void insert(int vpn, int pfn) {
        time_counter++;
        int set_idx = vpn % num_sets;
        int victim  = findVictim(set_idx);
        TLBEntry& e = sets[set_idx].ways[victim];
        e.vpn         = vpn;
        e.pfn         = pfn;
        e.valid       = true;
        e.last_used   = time_counter;
        e.insert_time = time_counter;
    }

    int    getTotalEntries()  const { return total_entries; }
    int    getAssociativity() const { return associativity; }
    int    getNumSets()       const { return num_sets; }
    std::string getPolicyName() const {
        if (policy == ReplacementPolicy::LRU)  return "LRU";
        if (policy == ReplacementPolicy::FIFO) return "FIFO";
        return "RANDOM";
    }
};

#endif // TLB_H