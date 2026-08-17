// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_blitter_zero.h for the primary calling header

#ifndef VERILATED_VAGNUS_BLITTER_ZERO___024ROOT_H_
#define VERILATED_VAGNUS_BLITTER_ZERO___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_blitter_zero__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_blitter_zero___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(clk7_en,0,0);
    VL_IN8(enable,0,0);
    VL_IN8(init,0,0);
    VL_IN8(store_result,0,0);
    VL_OUT8(zero,0,0);
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __VactContinue;
    VL_IN16(fill_out,15,0);
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_blitter_zero__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_blitter_zero___024root(Vagnus_blitter_zero__Syms* symsp, const char* v__name);
    ~Vagnus_blitter_zero___024root();
    VL_UNCOPYABLE(Vagnus_blitter_zero___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
