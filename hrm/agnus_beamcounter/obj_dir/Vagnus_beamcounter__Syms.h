// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VAGNUS_BEAMCOUNTER__SYMS_H_
#define VERILATED_VAGNUS_BEAMCOUNTER__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vagnus_beamcounter.h"

// INCLUDE MODULE CLASSES
#include "Vagnus_beamcounter___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vagnus_beamcounter__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vagnus_beamcounter* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vagnus_beamcounter___024root   TOP;

    // CONSTRUCTORS
    Vagnus_beamcounter__Syms(VerilatedContext* contextp, const char* namep, Vagnus_beamcounter* modelp);
    ~Vagnus_beamcounter__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
