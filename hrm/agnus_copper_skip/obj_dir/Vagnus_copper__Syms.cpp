// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper.h"
#include "Vagnus_copper___024root.h"
#include "Vagnus_copper_agnus_copper.h"

// FUNCTIONS
Vagnus_copper__Syms::~Vagnus_copper__Syms()
{
}

Vagnus_copper__Syms::Vagnus_copper__Syms(VerilatedContext* contextp, const char* namep, Vagnus_copper* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
    , TOP__agnus_copper{this, Verilated::catName(namep, "agnus_copper")}
{
        // Check resources
        Verilated::stackCheck(33);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    TOP.agnus_copper = &TOP__agnus_copper;
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
    TOP__agnus_copper.__Vconfigure(true);
    // Setup scopes
    __Vscope_agnus_copper.configure(this, name(), "agnus_copper", "agnus_copper", "<null>", 0, VerilatedScope::SCOPE_OTHER);
    // Setup export functions
    for (int __Vfinal = 0; __Vfinal < 2; ++__Vfinal) {
        __Vscope_agnus_copper.varInsert(__Vfinal,"dma_req", &(TOP__agnus_copper.dma_req), false, VLVT_UINT8,VLVD_NODIR|VLVF_PUB_RW,0,0);
        __Vscope_agnus_copper.varInsert(__Vfinal,"selins", &(TOP__agnus_copper.selins), false, VLVT_UINT8,VLVD_NODIR|VLVF_PUB_RW,0,0);
        __Vscope_agnus_copper.varInsert(__Vfinal,"selreg", &(TOP__agnus_copper.selreg), false, VLVT_UINT8,VLVD_NODIR|VLVF_PUB_RW,0,0);
    }
}
