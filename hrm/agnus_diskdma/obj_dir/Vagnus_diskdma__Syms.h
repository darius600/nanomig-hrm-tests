// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef VERILATED_VAGNUS_DISKDMA__SYMS_H_
#define VERILATED_VAGNUS_DISKDMA__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODEL CLASS

#include "Vagnus_diskdma.h"

// INCLUDE MODULE CLASSES
#include "Vagnus_diskdma___024root.h"

// SYMS CLASS (contains all model state)
class alignas(VL_CACHE_LINE_BYTES)Vagnus_diskdma__Syms final : public VerilatedSyms {
  public:
    // INTERNAL STATE
    Vagnus_diskdma* const __Vm_modelp;
    VlDeleter __Vm_deleter;
    bool __Vm_didInit = false;

    // MODULE INSTANCE STATE
    Vagnus_diskdma___024root       TOP;

    // CONSTRUCTORS
    Vagnus_diskdma__Syms(VerilatedContext* contextp, const char* namep, Vagnus_diskdma* modelp);
    ~Vagnus_diskdma__Syms();

    // METHODS
    const char* name() { return TOP.name(); }
};

#endif  // guard
