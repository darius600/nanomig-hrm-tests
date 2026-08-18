// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VAGNUS_BITPLANEDMA__SYMS_H_
#define VERILATED_VAGNUS_BITPLANEDMA__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vagnus_bitplanedma.h"

// INCLUDE MODULE CLASSES
#include "Vagnus_bitplanedma___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vagnus_bitplanedma__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vagnus_bitplanedma* const __Vm_modelp;
    bool __Vm_activity = false;  ///< Used by trace routines to determine change occurred
    uint32_t __Vm_baseCode = 0;  ///< Used by trace routines when tracing multiple models
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vagnus_bitplanedma___024root   TOP;

    // CONSTRUCTORS
    Vagnus_bitplanedma__Syms(VerilatedContext* contextp, const char* namep, Vagnus_bitplanedma* modelp);
    ~Vagnus_bitplanedma__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
