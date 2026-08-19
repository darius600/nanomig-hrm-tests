#include <iostream>
#include <iomanip>
#include "Vdenise_playfields.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise_playfields* dut = new Vdenise_playfields;

    std::cout << "Starting FPGA-029: Denise Playfield Generation Test...\n";

    dut->aga = 0;
    dut->bpldata = 0;
    dut->dblpf = 0;
    dut->pf2of = 0;
    dut->bplcon2 = 0;

    auto update = [&]() {
        dut->eval();
    };

    update();

    // Test 1: Single Playfield (dblpf=0)
    std::cout << "Testing Single Playfield...\n";
    dut->bpldata = 1; // bpldata[1] = 1, so color 1
    update();
    if (dut->plfdata != 1) {
        std::cerr << "FAIL: Single playfield expected plfdata=1, got " << (int)dut->plfdata << "\n";
        return 1;
    }
    if (dut->nplayfield != 2) { // 2 = bit 1 of [2:1] (so nplayfield[2]=1)
        std::cerr << "FAIL: Single playfield expected nplayfield[2]=1 (val 2), got " << (int)dut->nplayfield << "\n";
        return 1;
    }

    // Test 2: Dual Playfield, PF1 Priority (dblpf=1, bplcon2[6]=0)
    std::cout << "Testing Dual Playfield (PF1 Priority)...\n";
    dut->dblpf = 1;
    dut->bplcon2 = 0; // PF1 priority
    dut->bpldata = 3; // bpldata[1]=1 (PF1 color 1), bpldata[2]=1 (PF2 color 1)
    update();
    // PF1 priority: PF1 is chosen. plfdata = color 1. 
    // nplayfield[1]=1, nplayfield[2]=1 -> nplayfield = 3
    if (dut->plfdata != 1) {
        std::cerr << "FAIL: Dual PF (PF1 prio) expected plfdata=1, got " << (int)dut->plfdata << "\n";
        return 1;
    }
    if (dut->nplayfield != 3) {
        std::cerr << "FAIL: Dual PF expected nplayfield=3, got " << (int)dut->nplayfield << "\n";
        return 1;
    }

    // Test 3: Dual Playfield, PF2 Priority (dblpf=1, bplcon2[6]=1)
    std::cout << "Testing Dual Playfield (PF2 Priority)...\n";
    dut->bplcon2 = 0x40; // Bit 6 = 1 (PF2 priority)
    update();
    // PF2 priority: PF2 is chosen. In OCS, PF2 is offset by +8. So plfdata = 1 + 8 = 9.
    if (dut->plfdata != 9) {
        std::cerr << "FAIL: Dual PF (PF2 prio) expected plfdata=9, got " << (int)dut->plfdata << "\n";
        return 1;
    }

    // Test 4: OCS SWIV Undocumented Feature (dblpf=0, pf2p>5, bpldata[5]=1)
    std::cout << "Testing SWIV Undocumented Feature...\n";
    dut->dblpf = 0;
    dut->bplcon2 = 6 << 3; // pf2p = 6
    dut->bpldata = 0x10; // bpldata[5]=1 -> bit 4 of C++ uint8 is 0x10
    update();
    // Should force plfdata = 0x10 (16)
    if (dut->plfdata != 0x10) {
        std::cerr << "FAIL: SWIV mode expected plfdata=16, got " << (int)dut->plfdata << "\n";
        return 1;
    }

    std::cout << "PASS: All tests passed.\n";
    delete dut;
    return 0;
}
