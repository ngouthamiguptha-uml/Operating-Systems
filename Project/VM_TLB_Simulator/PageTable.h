#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <vector>
#include <unordered_map>
#include <stdexcept>

// ─────────────────────────────────────────────────────────────────────────────
//  Two-Level Page Table
//
//  Virtual address layout (32-bit assumed, 4 KB pages):
//    [31:22] = L1 index (10 bits, 1024 entries)
//    [21:12] = L2 index (10 bits, 1024 entries)
//    [11:0 ] = page offset (12 bits)
// ─────────────────────────────────────────────────────────────────────────────
class TwoLevelPageTable {
private:
    int l1_bits;
    int l2_bits;
    int l1_size;
    int l2_size;
    int page_offset_bits;

    // Sparse second-level tables: l2_tables[l1_idx][l2_idx] = pfn
    std::unordered_map<int, std::unordered_map<int,int>> l2_tables;

    int next_pfn; // next available physical frame

public:
    TwoLevelPageTable(int l1b = 10, int l2b = 10, int offset_bits = 12)
        : l1_bits(l1b), l2_bits(l2b), page_offset_bits(offset_bits), next_pfn(0)
    {
        l1_size = 1 << l1_bits;
        l2_size = 1 << l2_bits;
    }

    // Translate VPN → PFN (allocates new frame on first access)
    int getFrameByVPN(int vpn) {
        int l1_idx = (vpn >> l2_bits) & (l1_size - 1);
        int l2_idx = vpn & (l2_size - 1);
        auto& l2   = l2_tables[l1_idx];
        auto  it   = l2.find(l2_idx);
        if (it == l2.end()) {
            l2[l2_idx] = next_pfn++;
            return l2[l2_idx];
        }
        return it->second;
    }

    // Invalidate all L2 entries for a range of L1 indices (context switch)
    void flushProcess(int l1_start, int l1_end) {
        for (int i = l1_start; i <= l1_end && i < l1_size; i++)
            l2_tables.erase(i);
    }

    int getTotalAllocatedFrames() const { return next_pfn; }
};

// ─────────────────────────────────────────────────────────────────────────────
//  Unified PageTable wrapper — used by Simulator
// ─────────────────────────────────────────────────────────────────────────────
class PageTable {
private:
    TwoLevelPageTable impl;
public:
    PageTable() : impl(10, 10, 12) {}

    int getFrame(int vpn) {
        return impl.getFrameByVPN(vpn);
    }
};

#endif // PAGETABLE_H