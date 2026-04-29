#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "TLB.h"
#include "PageTable.h"
#include <vector>
#include <iostream>
#include <string>
#include <iomanip>

struct SimulationResult {
    int    total_accesses;
    int    tlb_hits;
    int    tlb_misses;
    int    context_switches;
    double hit_rate;
    double amat;

    std::string policy_name;
    std::string workload_name;
    std::string experiment_tag;   //for labelling experiments "EXP1" , "EXP2" , "EXP3"
    int    tlb_size;
    int    associativity;

    void print() const {
        std::cout << std::fixed << std::setprecision(4)
                  << "  Policy           : " << policy_name           << "\n"
                  << "  Workload         : " << workload_name         << "\n"
                  << "  TLB Size         : " << tlb_size              << "\n"
                  << "  Associativity    : " << associativity         << "-way\n"
                  << "  Total Accesses   : " << total_accesses        << "\n"
                  << "  TLB Hits         : " << tlb_hits              << "\n"
                  << "  TLB Misses       : " << tlb_misses            << "\n"
                  << "  Context Switches : " << context_switches      << "\n"
                  << "  Hit Rate         : " << hit_rate * 100.0      << " %\n"
                  << "  AMAT             : " << amat                  << " cycles\n";
    }
};

class Simulator {
private:
    int page_size; //4096 i.e 4 KB here, but could be parameterized if desired
    TLB tlb;
    PageTable page_table;
    int tlb_hits, tlb_misses, context_switches;

    //Assuming these timing costs for AMAT calculation (can be adjusted as needed)
    static constexpr double TLB_HIT_COST = 1.0; // Cost of a TLB hit (e.g., 1 cycle)
    static constexpr double PT_WALK_COST = 10.0; // Cost of a page table walk on a TLB miss (without considering memory access)
    static constexpr double MEM_ACCESS   = 100.0; // Cost of accessing memory after page table walk (e.g., on a TLB miss)

    double computeAMAT(double hr) const {
        return TLB_HIT_COST + (1.0 - hr) * (PT_WALK_COST + MEM_ACCESS); // AMAT = TLB HitCost + MissRate * MissPenalty
    }

public:
    Simulator(int tlb_size, int assoc, ReplacementPolicy pol, int psize = 4096)
        : page_size(psize),
          tlb(tlb_size, assoc, pol),
          tlb_hits(0), tlb_misses(0), context_switches(0)
    {}

    void run(const std::vector<int> &trace, int context_switch_interval = 0) { // context_switch_interval = 0 means no context switches
        tlb_hits = tlb_misses = context_switches = 0; // Reset counters before each run
        for (int i = 0; i < (int)trace.size(); i++) {
            if (context_switch_interval > 0 && // Context switching is enabled
                i > 0 && // Avoid flushing before the first access
                i % context_switch_interval == 0) { // Time to simulate a context switch i.e flush after N accesses.
                tlb.flush();
                context_switches++;
            }
            int vpn = trace[i] / page_size;
            int pfn = tlb.lookup(vpn);

            if (pfn != -1) { // TLB hit
                tlb_hits++;
            } else {
                tlb_misses++; // TLB miss: need to walk the page table to get the PFN and then insert into TLB
                pfn = page_table.getFrame(vpn);
                tlb.insert(vpn, pfn);
            }
        }
    }

    SimulationResult getResult(const std::string &workload_name) const {
        int total = tlb_hits + tlb_misses;
        double hr = (total > 0) ? (double)tlb_hits / total : 0.0;
        SimulationResult r;
        r.total_accesses   = total;
        r.tlb_hits         = tlb_hits;
        r.tlb_misses       = tlb_misses;
        r.context_switches = context_switches;
        r.hit_rate         = hr;
        r.amat             = computeAMAT(hr);
        r.policy_name      = tlb.getPolicyName();
        r.workload_name    = workload_name;
        r.experiment_tag   = "";
        r.tlb_size         = tlb.getTotalEntries();
        r.associativity    = tlb.getAssociativity();
        return r;
    }
};

#endif // SIMULATOR_H
