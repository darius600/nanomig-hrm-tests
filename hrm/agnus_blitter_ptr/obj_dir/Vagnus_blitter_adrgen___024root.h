// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_blitter_adrgen.h for the primary calling header

#ifndef VERILATED_VAGNUS_BLITTER_ADRGEN___024ROOT_H_
#define VERILATED_VAGNUS_BLITTER_ADRGEN___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_blitter_adrgen__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_blitter_adrgen___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(clk7_en,0,0);
    VL_IN8(reset,0,0);
    VL_IN8(ptrsel,1,0);
    VL_IN8(modsel,1,0);
    VL_IN8(enaptr,0,0);
    VL_IN8(incptr,0,0);
    VL_IN8(decptr,0,0);
    VL_IN8(addmod,0,0);
    VL_IN8(submod,0,0);
    VL_OUT8(sign_out,0,0);
    VL_IN8(reg_address_in,8,1);
    CData/*1:0*/ agnus_blitter_adrgen__DOT__bltptr_sel;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __VactContinue;
    VL_IN16(data_in,15,0);
    VL_OUT(address_out,20,1);
    IData/*19:0*/ agnus_blitter_adrgen__DOT__bltptr_in;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<CData/*4:0*/, 4> agnus_blitter_adrgen__DOT__bltpth;
    VlUnpacked<SData/*14:0*/, 4> agnus_blitter_adrgen__DOT__bltptl;
    VlUnpacked<SData/*14:0*/, 4> agnus_blitter_adrgen__DOT__bltmod;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_blitter_adrgen__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_blitter_adrgen___024root(Vagnus_blitter_adrgen__Syms* symsp, const char* v__name);
    ~Vagnus_blitter_adrgen___024root();
    VL_UNCOPYABLE(Vagnus_blitter_adrgen___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
