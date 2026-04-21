#ifndef WORKLOAD_H
#define WORKLOAD_H

#include <vector>
#include <random>
#include <algorithm>
#include <string>

class WorkloadGenerator {
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
                                     int page_size = 4096,
                                     int hot_pages = 50,
                                     int conflict_vpns = 80)
    {
        static std::mt19937 rng(42);
        std::vector<int> trace;
        trace.reserve(accesses);

        switch (type) {
        case WorkloadType::SEQUENTIAL:
            if (max_address <= 0) return trace;
            for (int i = 0; i < accesses; i++) {
                trace.push_back(((long long)i * page_size) % max_address);
            }
            break;

        case WorkloadType::RANDOM: {
            if (max_address <= 0) return trace;
            std::uniform_int_distribution<int> full_dist(0, max_address - 1);
            for (int i = 0; i < accesses; i++) {
                trace.push_back(full_dist(rng));
            }
            break;
        }

        case WorkloadType::LOOPING: {
            if (max_address <= 0) return trace;
            if (hot_pages <= 0) return trace;

            int hot_set_bytes = hot_pages * page_size;
            if (hot_set_bytes <= 0) return trace;

            std::uniform_int_distribution<int> full_dist(0, max_address - 1);
            std::uniform_int_distribution<int> hot_dist(0, hot_set_bytes - 1);

            int hot  = static_cast<int>(accesses * 0.80);
            int cold = accesses - hot;

            for (int i = 0; i < hot; i++) {
                trace.push_back(hot_dist(rng));
            }
            for (int i = 0; i < cold; i++) {
                trace.push_back(full_dist(rng));
            }

            std::shuffle(trace.begin(), trace.end(), rng);
            break;
        }

        case WorkloadType::WORKING_SET: {
            if (hot_pages <= 0) return trace;

            std::vector<int> hot_vpns;
            hot_vpns.reserve(hot_pages);

            for (int p = 0; p < hot_pages; p++) {
                hot_vpns.push_back(p * page_size);
            }

            for (int i = 0; i < accesses; i++) {
                trace.push_back(hot_vpns[i % hot_vpns.size()]);
            }
            break;
        }

        case WorkloadType::CONFLICT: {
            // This workload is designed to demonstrate associativity more logically.
            // Pages in each group differ by 64 VPNs, so they alias to the same set
            // for num_sets = 64, 32, 16, and 8.
            //
            // Group sizes:
            //   2-page group -> fits in 2-way and above
            //   3-page group -> fits in 4-way and above
            //   5-page group -> fits in 8-way and above
            //   7-page group -> strong pressure, still fits in fully-associative
            //
            // Total unique VPNs = 2 + 3 + 5 + 7 = 17 (< 64),
            // so fully-associative should approach 100% after warmup.

            const int stride = 64;

            std::vector<std::vector<int>> groups;

            auto make_group = [&](int base, int size) {
                std::vector<int> g;
                g.reserve(size);
                for (int k = 0; k < size; k++) {
                    int vpn = base + k * stride;
                    g.push_back(vpn * page_size);
                }
                return g;
            };

            // base values kept below 8 so mapping stays clean for all tested configs
            groups.push_back(make_group(0, 2)); // helps 2-way+
            groups.push_back(make_group(1, 3)); // helps 4-way+
            groups.push_back(make_group(2, 5)); // helps 8-way+
            groups.push_back(make_group(3, 9)); // strongest pressure

            while ((int)trace.size() < accesses) {
                for (const auto& g : groups) {
                    for (int addr : g) {
                        if ((int)trace.size() >= accesses) break;
                        trace.push_back(addr);
                    }
                }
            }
            break;
        }

        default: {
            if (max_address <= 0) return trace;
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