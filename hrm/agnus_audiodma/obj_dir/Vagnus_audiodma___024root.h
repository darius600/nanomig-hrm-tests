// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_audiodma.h for the primary calling header

#ifndef VERILATED_VAGNUS_AUDIODMA___024ROOT_H_
#define VERILATED_VAGNUS_AUDIODMA___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_audiodma__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_audiodma___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(clk7_en,0,0);
    VL_OUT8(dma,0,0);
    VL_IN8(audio_dmal,3,0);
    VL_IN8(audio_dmas,3,0);
    VL_IN8(reg_address_in,8,1);
    VL_OUT8(reg_address_out,8,1);
    CData/*0:0*/ agnus_audiodma__DOT__audlcena;
    CData/*1:0*/ agnus_audiodma__DOT__audlcsel;
    CData/*1:0*/ agnus_audiodma__DOT__channel;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __VactContinue;
    VL_IN16(hpos,8,0);
    VL_IN16(data_in,15,0);
    VL_OUT(address_out,20,1);
    IData/*19:0*/ agnus_audiodma__DOT__audptout;
    IData/*31:0*/ __VactIterCount;
    VlUnpacked<CData/*4:0*/, 4> agnus_audiodma__DOT__audlch;
    VlUnpacked<SData/*14:0*/, 4> agnus_audiodma__DOT__audlcl;
    VlUnpacked<IData/*19:0*/, 4> agnus_audiodma__DOT__audpt;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_audiodma__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_audiodma___024root(Vagnus_audiodma__Syms* symsp, const char* v__name);
    ~Vagnus_audiodma___024root();
    VL_UNCOPYABLE(Vagnus_audiodma___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
