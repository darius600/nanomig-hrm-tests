// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_blitter_mask.h for the primary calling header

#ifndef VERILATED_VAGNUS_BLITTER_MASK___024ROOT_H_
#define VERILATED_VAGNUS_BLITTER_MASK___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_blitter_mask__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_blitter_mask___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(first_word,0,0);
    VL_IN8(last_word,0,0);
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __VactContinue;
    VL_IN16(bltafwm,15,0);
    VL_IN16(bltalwm,15,0);
    VL_IN16(bltadat,15,0);
    VL_OUT16(mask_out,15,0);
    VL_OUT16(data_out,15,0);
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<0> __VactTriggered;
    VlTriggerVec<0> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_blitter_mask__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_blitter_mask___024root(Vagnus_blitter_mask__Syms* symsp, const char* v__name);
    ~Vagnus_blitter_mask___024root();
    VL_UNCOPYABLE(Vagnus_blitter_mask___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
