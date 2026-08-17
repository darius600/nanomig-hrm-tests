// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vagnus_blitter_line_octant__pch.h"
#include "Vagnus_blitter_line_octant.h"
#include "Vagnus_blitter_line_octant___024root.h"

// FUNCTIONS
Vagnus_blitter_line_octant__Syms::~Vagnus_blitter_line_octant__Syms()
{
}

Vagnus_blitter_line_octant__Syms::Vagnus_blitter_line_octant__Syms(VerilatedContext* contextp, const char* namep, Vagnus_blitter_line_octant* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
        // Check resources
        Verilated::stackCheck(33);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
