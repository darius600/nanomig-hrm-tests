// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table implementation internals

#include "Vagnus_bitplanedma__pch.h"
#include "Vagnus_bitplanedma.h"
#include "Vagnus_bitplanedma___024root.h"

// FUNCTIONS
Vagnus_bitplanedma__Syms::~Vagnus_bitplanedma__Syms()
{
}

Vagnus_bitplanedma__Syms::Vagnus_bitplanedma__Syms(VerilatedContext* contextp, const char* namep, Vagnus_bitplanedma* modelp)
    : VerilatedSyms{contextp}
    // Setup internal state of the Syms class
    , __Vm_modelp{modelp}
    // Setup module instances
    , TOP{this, namep}
{
        // Check resources
        Verilated::stackCheck(97);
    // Configure time unit / time precision
    _vm_contextp__->timeunit(-12);
    _vm_contextp__->timeprecision(-12);
    // Setup each module's pointers to their submodules
    // Setup each module's pointer back to symbol table (for public functions)
    TOP.__Vconfigure(true);
}
