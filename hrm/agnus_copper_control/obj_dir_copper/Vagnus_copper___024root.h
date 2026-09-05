// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_copper.h for the primary calling header

#ifndef VERILATED_VAGNUS_COPPER___024ROOT_H_
#define VERILATED_VAGNUS_COPPER___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_copper__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_copper___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(clk7_en,0,0);
    VL_IN8(reset,0,0);
    VL_IN8(ecs,0,0);
    VL_OUT8(reqdma,0,0);
    VL_IN8(ackdma,0,0);
    VL_IN8(enadma,0,0);
    VL_IN8(sof,0,0);
    VL_IN8(blit_busy,0,0);
    VL_IN8(vpos,7,0);
    VL_IN8(reg_address_in,8,1);
    VL_OUT8(reg_address_out,8,1);
    CData/*4:0*/ agnus_copper__DOT__cop1lch;
    CData/*4:0*/ agnus_copper__DOT__cop2lch;
    CData/*0:0*/ agnus_copper__DOT__cdang;
    CData/*2:0*/ agnus_copper__DOT__copper_state;
    CData/*2:0*/ agnus_copper__DOT__copper_next;
    CData/*0:0*/ agnus_copper__DOT__strobe1;
    CData/*0:0*/ agnus_copper__DOT__strobe2;
    CData/*0:0*/ agnus_copper__DOT__strobe;
    CData/*0:0*/ agnus_copper__DOT__skip_flag;
    CData/*0:0*/ agnus_copper__DOT__selins;
    CData/*0:0*/ agnus_copper__DOT__selreg;
    CData/*0:0*/ agnus_copper__DOT__skip;
    CData/*0:0*/ agnus_copper__DOT__enable;
    CData/*0:0*/ agnus_copper__DOT__dma_ack;
    CData/*0:0*/ agnus_copper__DOT__beam_match;
    CData/*0:0*/ agnus_copper__DOT__beam_match_skip;
    CData/*0:0*/ agnus_copper__DOT__beam_match_wait;
    CData/*0:0*/ agnus_copper__DOT__bus_ena;
    CData/*0:0*/ agnus_copper__DOT__bus_blk;
    CData/*0:0*/ agnus_copper__DOT__copjmp1;
    CData/*0:0*/ agnus_copper__DOT__copjmp2;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __VactContinue;
    VL_IN16(hpos,8,0);
    VL_IN16(data_in,15,0);
    SData/*14:0*/ agnus_copper__DOT__cop1lcl;
    SData/*14:0*/ agnus_copper__DOT__cop2lcl;
    SData/*14:0*/ agnus_copper__DOT__ir1;
    SData/*15:0*/ agnus_copper__DOT__ir2;
    VL_OUT(address_out,20,1);
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_copper__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_copper___024root(Vagnus_copper__Syms* symsp, const char* v__name);
    ~Vagnus_copper___024root();
    VL_UNCOPYABLE(Vagnus_copper___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
