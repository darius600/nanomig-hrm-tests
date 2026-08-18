// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VAGNUS_COPPER__SYMS_H_
#define VERILATED_VAGNUS_COPPER__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vagnus_copper.h"

// INCLUDE MODULE CLASSES
#include "Vagnus_copper___024root.h"
#include "Vagnus_copper_agnus_copper.h"

// DPI TYPES for DPI Export callbacks (Internal use)

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vagnus_copper__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vagnus_copper* const __Vm_modelp;
    bool __Vm_activity = false;  ///< Used by trace routines to determine change occurred
    uint32_t __Vm_baseCode = 0;  ///< Used by trace routines when tracing multiple models
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vagnus_copper___024root        TOP;
    Vagnus_copper_agnus_copper     TOP__agnus_copper;

    // SCOPE NAMES
    VerilatedScope __Vscope_agnus_copper;

    // CONSTRUCTORS
    Vagnus_copper__Syms(VerilatedContext* contextp, const char* namep, Vagnus_copper* modelp);
    ~Vagnus_copper__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
