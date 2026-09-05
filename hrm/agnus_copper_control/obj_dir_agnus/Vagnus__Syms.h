// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VAGNUS__SYMS_H_
#define VERILATED_VAGNUS__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vagnus.h"

// INCLUDE MODULE CLASSES
#include "Vagnus___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vagnus__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vagnus* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vagnus___024root               TOP;

    // CONSTRUCTORS
    Vagnus__Syms(VerilatedContext* contextp, const char* namep, Vagnus* modelp);
    ~Vagnus__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
