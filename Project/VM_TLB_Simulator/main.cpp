#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include "Simulator.h"
#include "WorkloadGenerator.h"

void printDivider(char c = '-', int n = 70) {
    std::cout << std::string(n, c) << "\n";
}
void printHeader(const std::string &title) {
    printDivider('=');
    std::cout << "  " << title << "\n";
    printDivider('=');
}

void writeCSV(const std::vector<SimulationResult> &results,
              const std::string &filename = "results.csv") // Writes results to a CSV file
{
    std::ofstream f(filename);
    f << "experiment,workload,policy,tlb_size,associativity,hit_rate_pct,amat,context_switches\n"; // CSV Column names
    for (const auto &r : results) {
        f << r.experiment_tag   << ","
          << r.workload_name    << ","
          << r.policy_name      << ","
          << r.tlb_size         << ","
          << r.associativity    << ","
          << std::fixed << std::setprecision(4) << (r.hit_rate * 100.0) << ","
          << r.amat             << ","
          << r.context_switches << "\n";
    }
    std::cout << "[CSV] Written to " << filename << "\n";
}

// ── Exp 1: Policy × Workload Comparison─────────────────────────────────────────────────
// 128 VPNs total with a 64-entry TLB.
// Small enough to show reuse effects, but large enough that the TLB cannot hold all pages.
std::vector<SimulationResult> exp_policy(int accesses, int page_size) {
    printHeader("EXP 1: Policy Comparison Under 3 Workloads (64-entry, 4-way)");

    const int max_addr  = 128 * page_size; // 128 unique VPNs
    const int hot_pages = 50;              // Looping: 50 hot VPNs out of 128 total

    using WT = WorkloadGenerator::WorkloadType;
    std::vector<WT> workloads = { WT::SEQUENTIAL, WT::RANDOM, WT::LOOPING };
    std::vector<ReplacementPolicy> policies = {
        ReplacementPolicy::LRU, ReplacementPolicy::FIFO, ReplacementPolicy::RANDOM
    };

    std::vector<SimulationResult> results; // To store results for all policy-workload combinations
   
    for (auto wt : workloads) { //Loop over each workload type
        auto trace = WorkloadGenerator::generate(accesses, max_addr, wt, page_size, hot_pages); //Generate memory trace for that workload
        std::string wname = WorkloadGenerator::typeName(wt);
        for (auto pol : policies) { //For the same trace, test all three policies
            Simulator sim(64, 4, pol, page_size); //Create a simulator with 64-entry TLB, 4-way associativity, and the current replacement policy
            sim.run(trace);
            auto r = sim.getResult(wname);
            r.experiment_tag = "EXP1";
            printDivider();
            std::cout << "  [" << wname << " | " << r.policy_name << "]\n";
            r.print();
            results.push_back(r);
        }
    }
    return results;
}

// ── Exp 2: Associativity Comparison ──────────────────────────────────────────────────────
// This experiment tests how associativity affects TLB hit rate.
// CONFLICT workload cycles VPNs 0..79 in strict order (80 unique VPNs, 64-entry TLB).
// set_idx = vpn % num_sets, so:
//   Direct-Mapped (64 sets, 1-way) : 16 sets alias → ~0-10% hit rate
//   2-way  (32 sets)               : 80/32=2.5 per set → ~40-50%
//   4-way  (16 sets)               : 80/16=5 per set  → ~60-70%
//   8-way  ( 8 sets)               : 80/8=10 per set  → ~75%
//   Fully-Assoc (1 set, 64-way)    : holds 64 of 80   → ~80%
std::vector<SimulationResult> exp_assoc(int accesses, int page_size) {
    printHeader("EXP 2: Associativity Comparison (LRU, Mixed Conflict Workload)"); // Uses LRU policy, 64-entry TLB, and a custom CONFLICT workload

    auto trace = WorkloadGenerator::generate(
        accesses,
        1, // unused for CONFLICT in this design
        WorkloadGenerator::WorkloadType::CONFLICT,
        page_size
    );

    std::vector<int> assoc_levels = {1, 2, 4, 8, 64};
    
    std::vector<SimulationResult> results;

    for (int assoc : assoc_levels) {
        if (64 % assoc != 0) continue; // Skip invalid configurations where TLB size is not divisible by associativity

        Simulator sim(64, assoc, ReplacementPolicy::LRU, page_size); // Create a simulator with 64-entry TLB, current associativity & LRU policy
        sim.run(trace);

        std::string label = (assoc == 1  ? "Direct-Mapped" :
                             assoc == 64 ? "Fully-Assoc"   :
                             std::to_string(assoc) + "-way");

        auto r = sim.getResult("AssocConflict");
        r.workload_name  = label;
        r.experiment_tag = "EXP2";

        printDivider();
        std::cout << " [" << label << "]\n";
        r.print();

        results.push_back(r);
    }
    return results;
}

// ── Exp 3: Context Switch Impact─────────────────────────────────────────────────────
// This experiment tests how TLB flushing affects performance.
// 512 VPNs random — enough reuse that TLB warmup matters
// Each flush kills warm TLB entries, clearly visible at high flush frequencies
std::vector<SimulationResult> exp_ctx(int accesses, int page_size) {
    printHeader("EXP 3: Context Switch Impact (LRU, 4-way, Working_Set)");

    const int max_addr = 80 * page_size;
    
    auto trace = WorkloadGenerator::generate(accesses, max_addr,
                     WorkloadGenerator::WorkloadType::WORKING_SET, page_size); // Generate a working set workload with 80 unique VPNs

    std::vector<int> intervals = {0, 10000, 5000, 2000, 500}; // Context switch intervals (0 means no context switches, 10,000 means flush every 10k accesses, etc.)
    
    std::vector<SimulationResult> results;

    for (int iv : intervals) {
        Simulator sim(64, 4, ReplacementPolicy::LRU, page_size); // Create a simulator with 64-entry TLB, 4-way associativity, LRU policy, and the specified page size.
        sim.run(trace, iv);
        std::string label = (iv == 0) ? "No-Switch" :
                             "Switch-" + std::to_string(iv);
        auto r = sim.getResult(label);
        r.workload_name  = label;
        r.experiment_tag = "EXP3";
        printDivider();
        std::cout << "  [Context switch every " << iv << " accesses]\n";
        r.print();
        results.push_back(r);
    }
    return results;
}

int main() {
    const int accesses  = 200000; // Total number of memory accesses to simulate in each experiment
    const int page_size = 4096; // 4KB page size

    printDivider('*');
    std::cout << "  VM TLB Simulator — Final\n"
              << "  Page size: " << page_size << " B  |  Accesses: " << accesses << "\n";
    printDivider('*');

    std::vector<SimulationResult> all; // Store results from all experiments for later CSV output

    // Run all three experiments and collect results
    auto r1 = exp_policy(accesses, page_size);
    auto r2 = exp_assoc (accesses, page_size);
    auto r3 = exp_ctx   (accesses, page_size);
    all.insert(all.end(), r1.begin(), r1.end());
    all.insert(all.end(), r2.begin(), r2.end());
    all.insert(all.end(), r3.begin(), r3.end());

    writeCSV(all);
    printHeader("DONE — run: python3 visualize.py");
    return 0;
}