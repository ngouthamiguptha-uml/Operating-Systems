#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <vector>

/*
Single Level Page Table
Maps virtual page numbers to physical frames
*/

class PageTable {

private:
    std::vector<int> table;

public:

    PageTable(int num_pages) {
        table.resize(num_pages);

        for (int i = 0; i < num_pages; i++)
            table[i] = i;      // simple mapping VPN -> PFN
    }

    /*
    Translate VPN to PFN
    */
    int getFrame(int vpn) {
        return table[vpn];
    }
};

#endif