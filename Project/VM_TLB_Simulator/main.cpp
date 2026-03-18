#include <iostream>
#include "Simulator.h"
#include "WorkloadGenerator.h"

int main() {

    int accesses = 100000;
    int max_address = 65536;
    int page_size = 4096;
    int num_pages = max_address / page_size;

    /*
    Generate synthetic workload
    */
    auto trace = WorkloadGenerator::generate(accesses, max_address);

    std::cout << "===== TLB Size 16 =====" << std::endl;

    Simulator sim1(16, num_pages, page_size);
    sim1.run(trace);
    sim1.printStats();


    std::cout << "\n===== TLB Size 64 =====" << std::endl;

    Simulator sim2(64, num_pages, page_size);
    sim2.run(trace);
    sim2.printStats();

}