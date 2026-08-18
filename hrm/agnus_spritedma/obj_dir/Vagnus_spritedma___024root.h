// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_spritedma.h for the primary calling header

#ifndef VERILATED_VAGNUS_SPRITEDMA___024ROOT_H_
#define VERILATED_VAGNUS_SPRITEDMA___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_spritedma__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_spritedma___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(clk7_en,0,0);
    VL_IN8(reset,0,0);
    VL_IN8(aga,0,0);
    VL_IN8(ecs,0,0);
    VL_OUT8(reqdma,0,0);
    VL_IN8(ackdma,0,0);
    VL_IN8(vbl,0,0);
    VL_IN8(vblend,0,0);
    VL_IN8(reg_address_in,8,1);
    VL_OUT8(reg_address_out,8,1);
    CData/*2:0*/ agnus_spritedma__DOT__sprite;
    CData/*0:0*/ agnus_spritedma__DOT__enable;
    CData/*0:0*/ agnus_spritedma__DOT__sprvstop;
    CData/*0:0*/ agnus_spritedma__DOT__sprdmastate;
    CData/*0:0*/ agnus_spritedma__DOT__dmastate_in;
    CData/*2:0*/ agnus_spritedma__DOT__sprsel;
    CData/*2:0*/ agnus_spritedma__DOT__ptsel;
    CData/*2:0*/ agnus_spritedma__DOT__pcsel;
    CData/*0:0*/ agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_0;
    CData/*0:0*/ agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_1;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __VactContinue;
    VL_IN16(hpos,8,0);
    VL_IN16(vpos,10,0);
    VL_IN16(data_in,15,0);
    SData/*9:0*/ agnus_spritedma__DOT__vstop;
    SData/*15:0*/ agnus_spritedma__DOT__fmode;
    SData/*8:0*/ agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_3;
    SData/*8:0*/ agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_4;
    VL_OUT(address_out,20,1);
    IData/*19:0*/ agnus_spritedma__DOT__newptr;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<CData/*4:0*/, 8> agnus_spritedma__DOT__sprpth;
    VlUnpacked<SData/*14:0*/, 8> agnus_spritedma__DOT__sprptl;
    VlUnpacked<CData/*7:0*/, 8> agnus_spritedma__DOT__sprpos;
    VlUnpacked<CData/*0:0*/, 8> agnus_spritedma__DOT__sprposh;
    VlUnpacked<SData/*11:0*/, 8> agnus_spritedma__DOT__sprctl;
    VlUnpacked<CData/*0:0*/, 8> agnus_spritedma__DOT__dmastate_mem;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_spritedma__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_spritedma___024root(Vagnus_spritedma__Syms* symsp, const char* v__name);
    ~Vagnus_spritedma___024root();
    VL_UNCOPYABLE(Vagnus_spritedma___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
