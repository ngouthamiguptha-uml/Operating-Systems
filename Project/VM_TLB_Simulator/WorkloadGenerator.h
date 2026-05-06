#ifndef WORKLOAD_H
#define WORKLOAD_H

#include <vector>
#include <random>
#include <algorithm>
#include <string>

class WorkloadGenerator { // Generates memory access traces with different patterns to test TLB performance
public:
    enum class WorkloadType {
        SEQUENTIAL,
        RANDOM,
        LOOPING,
        WORKING_SET,
        CONFLICT
    };

    // Return type is std::vector<int> to match Simulator::run(const std::vector<int>&)
    static std::vector<int> generate(int accesses, int max_address,
                                     WorkloadType type = WorkloadType::RANDOM,
                                     int page_size = 4096, // default 4 KB pages
                                     int hot_pages = 50) // used for LOOPING and WORKING_SET. Number of frequently reused pages.
                                     
    {
        static std::mt19937 rng(42); //Mersenne Twister random number generator seeded with a fixed value for reproducibility
        
        std::vector<int> trace; //creates an empty vector to hold the generated memory access trace
        trace.reserve(accesses); //reserves memory for the specified number of accesses (pre-allocates memory)

        switch (type) {
        case WorkloadType::SEQUENTIAL:
            if (max_address <= 0) return trace; // Guard against invalid max_address
            for (int i = 0; i < accesses; i++) {
                trace.push_back(((long long)i * page_size) % max_address);
            }
            break;

        case WorkloadType::RANDOM: {
            if (max_address <= 0) return trace; // Guard against invalid max_address
            std::uniform_int_distribution<int> full_dist(0, max_address - 1); //creates a uniform distribution to generate random addresses between 0 and max_address - 1
            for (int i = 0; i < accesses; i++) {
                trace.push_back(full_dist(rng));
            }
            break;
        }

        case WorkloadType::LOOPING: {
            if (max_address <= 0) return trace; // Guard against invalid max_address
            if (hot_pages <= 0) return trace; // Guard against invalid hot_pages

            int hot_set_bytes = hot_pages * page_size;
            if (hot_set_bytes <= 0) return trace; // Guard against overflow or invalid hot_set_bytes

            std::uniform_int_distribution<int> full_dist(0, max_address - 1);
            std::uniform_int_distribution<int> hot_dist(0, hot_set_bytes - 1);

            int hot  = static_cast<int>(accesses * 0.80); // 80% of accesses target hot pages
            int cold = accesses - hot; // 20% of accesses target cold pages

            for (int i = 0; i < hot; i++) {
                trace.push_back(hot_dist(rng));
            }
            for (int i = 0; i < cold; i++) {
                trace.push_back(full_dist(rng));
            }

            std::shuffle(trace.begin(), trace.end(), rng); // Randomize the order of accesses to mix hot and cold pages
            break;
        }

        case WorkloadType::WORKING_SET: {
            int hot_pages = max_address / page_size; //80 hot pages by default

            if (hot_pages <= 0) return trace; // Guard against invalid or no hot_pages

            std::vector<int> hot_addrs; //Creates a vector of hot addresses
            hot_addrs.reserve(hot_pages);

            for (int p = 0; p < hot_pages; p++) {
                hot_addrs.push_back(p * page_size);
            }

            for (int i = 0; i < accesses; i++) { //Generates accesses by repeatedly cycling through the hot addresses
               // int idx = i % hot_pages; // cycle through working set
                int idx = rand() % hot_pages; // randomize within working set
                trace.push_back(hot_addrs[idx]);                
            }
            break;
        }

        case WorkloadType::CONFLICT: {
            // This workload is designed to demonstrate associativity more logically.
            // It creates pages that map to the same TLB sets.

            const int stride = 64; //because TLB size is 64 entries, this stride will cause all addresses to map to the same set in a direct-mapped TLB, and create varying levels of conflict for higher associativities.

            std::vector<std::vector<int>> groups; //stores groups of addresses that will conflict with each other (maps to same set index of TLB) in the TLB. 
            auto make_group = [&](int base, int size) { //base is the starting page number, size is how many pages in the group.
                
                std::vector<int> g;
                g.reserve(size);
                
                for (int k = 0; k < size; k++) {
                    int vpn = base + k * stride;
                    g.push_back(vpn * page_size); //convert VPN to a virtual address
                }
                return g;
            };

            // test groups designed to create different levels of conflict for different associativities:
            groups.push_back(make_group(0, 2));  // Two pages competing in one set
            groups.push_back(make_group(1, 3)); 
            groups.push_back(make_group(2, 5)); 
            groups.push_back(make_group(3, 9)); 

            while ((int)trace.size() < accesses) {
                for (const auto &g : groups) { //Loop through each conflict group
                    for (int addr : g) { //Loop through each address in the group
                        if ((int)trace.size() >= accesses) break;
                        trace.push_back(addr);
                    }
                }
            }
            break;
        }

        default: {
            if (max_address <= 0) return trace; // Guard against invalid max_address
            std::uniform_int_distribution<int> full_dist(0, max_address - 1);
            for (int i = 0; i < accesses; i++) {
                trace.push_back(full_dist(rng));
            }
            break;
        }
        }

        return trace;
    }

    static std::string typeName(WorkloadType t) {
        if (t == WorkloadType::SEQUENTIAL)  return "Sequential";
        if (t == WorkloadType::LOOPING)     return "Looping";
        if (t == WorkloadType::WORKING_SET) return "WorkingSet";
        if (t == WorkloadType::CONFLICT)    return "Conflict";
        return "Random";
    }
};

#endif // WORKLOAD_H