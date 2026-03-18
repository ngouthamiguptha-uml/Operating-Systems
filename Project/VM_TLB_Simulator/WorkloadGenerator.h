#ifndef WORKLOAD_H
#define WORKLOAD_H

#include <vector>
#include <cstdlib>

/*
Generates synthetic workload of memory accesses
*/

class WorkloadGenerator {

public:

    static std::vector<int> generate(int accesses, int max_address) {

        std::vector<int> trace;

        for (int i = 0; i < accesses; i++) {

            int addr = rand() % max_address;
            trace.push_back(addr);
        }

        return trace;
    }
};

#endif