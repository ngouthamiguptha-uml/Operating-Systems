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
    std::string experiment_tag;   // ← NEW: "EXP1" / "EXP2" / "EXP3"
    int    tlb_size;
    int    associativity;

    void print() const {
        std::cout << std::fixed << std::setprecision(4)
                  << "  Policy        : " << policy_name           << "\n"
                  << "  Workload      : " << workload_name         << "\n"
                  << "  TLB Size      : " << tlb_size              << "\n"
                  << "  Associativity : " << associativity << "-way\n"
                  << "  Total Accesses: " << total_accesses        << "\n"
                  << "  TLB Hits      : " << tlb_hits              << "\n"
                  << "  TLB Misses    : " << tlb_misses            << "\n"
                  << "  Ctx Switches  : " << context_switches      << "\n"
                  << "  Hit Rate      : " << hit_rate * 100.0 << " %\n"
                  << "  AMAT          : " << amat << " cycles\n";
    }
};

class Simulator {
private:
    int page_size;
    TLB tlb;
    PageTable page_table;
    int tlb_hits, tlb_misses, context_switches;

    static constexpr double TLB_HIT_COST = 1.0;
    static constexpr double PT_WALK_COST = 10.0;
    static constexpr double MEM_ACCESS   = 100.0;

    double computeAMAT(double hr) const {
        return TLB_HIT_COST + (1.0 - hr) * (PT_WALK_COST + MEM_ACCESS);
    }

public:
    Simulator(int tlb_size, int assoc, ReplacementPolicy pol, int psize = 4096)
        : page_size(psize),
          tlb(tlb_size, assoc, pol),
          tlb_hits(0), tlb_misses(0), context_switches(0)
    {}

    void run(const std::vector<int>& trace, int context_switch_interval = 0) {
        tlb_hits = tlb_misses = context_switches = 0;
        for (int i = 0; i < (int)trace.size(); i++) {
            if (context_switch_interval > 0 && i > 0 &&
                i % context_switch_interval == 0) {
                tlb.flush();
                context_switches++;
            }
            int vpn = trace[i] / page_size;
            int pfn = tlb.lookup(vpn);
            if (pfn != -1) {
                tlb_hits++;
            } else {
                tlb_misses++;
                pfn = page_table.getFrame(vpn);
                tlb.insert(vpn, pfn);
            }
        }
    }

    SimulationResult getResult(const std::string& workload_name) const {
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
