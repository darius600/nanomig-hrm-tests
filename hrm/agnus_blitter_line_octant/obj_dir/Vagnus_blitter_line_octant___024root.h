// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_blitter_line_octant.h for the primary calling header

#ifndef VERILATED_VAGNUS_BLITTER_LINE_OCTANT___024ROOT_H_
#define VERILATED_VAGNUS_BLITTER_LINE_OCTANT___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_blitter_line_octant__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_blitter_line_octant___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(bltcon1,4,2);
    VL_IN8(sign_del,0,0);
    VL_IN8(ash,3,0);
    VL_OUT8(lineinc,0,0);
    VL_OUT8(linedec,0,0);
    VL_OUT8(lineadd,0,0);
    VL_OUT8(linesub,0,0);
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __VactContinue;
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<0> __VactTriggered;
    VlTriggerVec<0> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_blitter_line_octant__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_blitter_line_octant___024root(Vagnus_blitter_line_octant__Syms* symsp, const char* v__name);
    ~Vagnus_blitter_line_octant___024root();
    VL_UNCOPYABLE(Vagnus_blitter_line_octant___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
