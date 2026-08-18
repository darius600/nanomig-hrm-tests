// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_refresh.h for the primary calling header

#ifndef VERILATED_VAGNUS_REFRESH___024ROOT_H_
#define VERILATED_VAGNUS_REFRESH___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_refresh__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_refresh___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_OUT8(dma,0,0);
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __VactContinue;
    VL_IN16(hpos,8,0);
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<0> __VactTriggered;
    VlTriggerVec<0> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_refresh__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_refresh___024root(Vagnus_refresh__Syms* symsp, const char* v__name);
    ~Vagnus_refresh___024root();
    VL_UNCOPYABLE(Vagnus_refresh___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
