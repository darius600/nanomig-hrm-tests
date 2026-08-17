// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VAGNUS_BLITTER_LINE_OCTANT__SYMS_H_
#define VERILATED_VAGNUS_BLITTER_LINE_OCTANT__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vagnus_blitter_line_octant.h"

// INCLUDE MODULE CLASSES
#include "Vagnus_blitter_line_octant___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vagnus_blitter_line_octant__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vagnus_blitter_line_octant* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vagnus_blitter_line_octant___024root TOP;

    // CONSTRUCTORS
    Vagnus_blitter_line_octant__Syms(VerilatedContext* contextp, const char* namep, Vagnus_blitter_line_octant* modelp);
    ~Vagnus_blitter_line_octant__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
