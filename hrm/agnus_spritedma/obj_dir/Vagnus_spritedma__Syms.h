// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VAGNUS_SPRITEDMA__SYMS_H_
#define VERILATED_VAGNUS_SPRITEDMA__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vagnus_spritedma.h"

// INCLUDE MODULE CLASSES
#include "Vagnus_spritedma___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vagnus_spritedma__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vagnus_spritedma* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vagnus_spritedma___024root     TOP;

    // CONSTRUCTORS
    Vagnus_spritedma__Syms(VerilatedContext* contextp, const char* namep, Vagnus_spritedma* modelp);
    ~Vagnus_spritedma__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
