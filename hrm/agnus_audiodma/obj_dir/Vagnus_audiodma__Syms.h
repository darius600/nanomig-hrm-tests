// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VAGNUS_AUDIODMA__SYMS_H_
#define VERILATED_VAGNUS_AUDIODMA__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vagnus_audiodma.h"

// INCLUDE MODULE CLASSES
#include "Vagnus_audiodma___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vagnus_audiodma__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vagnus_audiodma* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vagnus_audiodma___024root      TOP;

    // CONSTRUCTORS
    Vagnus_audiodma__Syms(VerilatedContext* contextp, const char* namep, Vagnus_audiodma* modelp);
    ~Vagnus_audiodma__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
