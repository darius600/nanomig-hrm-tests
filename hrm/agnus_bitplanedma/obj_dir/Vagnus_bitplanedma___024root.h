// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_bitplanedma.h for the primary calling header

#ifndef VERILATED_VAGNUS_BITPLANEDMA___024ROOT_H_
#define VERILATED_VAGNUS_BITPLANEDMA___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_bitplanedma__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_bitplanedma___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    // Anonymous structures to workaround compiler member-count bugs
    struct {
        VL_IN8(clk,0,0);
        VL_IN8(clk7_en,0,0);
        VL_IN8(reset,0,0);
        VL_IN8(harddis,0,0);
        VL_IN8(aga,0,0);
        VL_IN8(ecs,0,0);
        VL_IN8(a1k,0,0);
        VL_IN8(sof,0,0);
        VL_IN8(dmaena,0,0);
        VL_OUT8(hde,0,0);
        VL_OUT8(dma,0,0);
        VL_IN8(reg_address_in,8,1);
        VL_OUT8(reg_address_out,8,1);
        CData/*6:0*/ agnus_bitplanedma__DOT__ddfstrt;
        CData/*6:0*/ agnus_bitplanedma__DOT__ddfstop;
        CData/*6:0*/ agnus_bitplanedma__DOT__ddfdiff;
        CData/*6:0*/ agnus_bitplanedma__DOT__ddfdiff_masked;
        CData/*5:0*/ agnus_bitplanedma__DOT__bplcon0;
        CData/*5:0*/ agnus_bitplanedma__DOT__bplcon0_delayed;
        CData/*0:0*/ agnus_bitplanedma__DOT__shres;
        CData/*4:0*/ agnus_bitplanedma__DOT__plane;
        CData/*0:0*/ agnus_bitplanedma__DOT__hardena;
        CData/*0:0*/ agnus_bitplanedma__DOT__softena;
        CData/*0:0*/ agnus_bitplanedma__DOT__ddfena;
        CData/*0:0*/ agnus_bitplanedma__DOT__ddfena_0;
        CData/*4:0*/ agnus_bitplanedma__DOT__ddfseq;
        CData/*0:0*/ agnus_bitplanedma__DOT__ddfrun;
        CData/*0:0*/ agnus_bitplanedma__DOT__ddfend;
        CData/*1:0*/ agnus_bitplanedma__DOT__dmaena_delayed;
        CData/*0:0*/ agnus_bitplanedma__DOT__vdiwena;
        CData/*2:0*/ agnus_bitplanedma__DOT__bplptr_sel;
        CData/*0:0*/ agnus_bitplanedma__DOT__bp_fmode12;
        CData/*0:0*/ agnus_bitplanedma__DOT__soft_start;
        CData/*0:0*/ agnus_bitplanedma__DOT__soft_stop;
        CData/*0:0*/ agnus_bitplanedma__DOT__hard_start;
        CData/*0:0*/ agnus_bitplanedma__DOT__hard_stop;
        CData/*0:0*/ agnus_bitplanedma__DOT__ddfseq_match;
        CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7;
        CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9;
        CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11;
        CData/*2:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14;
        CData/*0:0*/ __VstlFirstIteration;
        CData/*0:0*/ __VicoFirstIteration;
        CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
        CData/*0:0*/ __VactContinue;
        VL_IN16(vpos,10,0);
        VL_IN16(hpos,8,0);
        VL_IN16(data_in,15,0);
        SData/*14:0*/ agnus_bitplanedma__DOT__bpl1mod;
        SData/*14:0*/ agnus_bitplanedma__DOT__bpl2mod;
        SData/*14:0*/ agnus_bitplanedma__DOT__bpl1mod_bscan;
        SData/*14:0*/ agnus_bitplanedma__DOT__bpl2mod_bscan;
        SData/*15:0*/ agnus_bitplanedma__DOT__fmode;
        SData/*10:0*/ agnus_bitplanedma__DOT__vdiwstrt;
        SData/*10:0*/ agnus_bitplanedma__DOT__vdiwstop;
        SData/*8:0*/ agnus_bitplanedma__DOT__hdiwstrt;
        SData/*8:0*/ agnus_bitplanedma__DOT__hdiwstop;
        SData/*8:0*/ agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt;
        SData/*8:0*/ agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstrt;
        SData/*8:0*/ agnus_bitplanedma__DOT__unnamedblk1__DOT__prev_hdiwstrt;
        SData/*8:0*/ agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop;
        SData/*8:0*/ agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstop;
        SData/*8:0*/ agnus_bitplanedma__DOT__unnamedblk1__DOT__prev_hdiwstop;
        SData/*10:0*/ agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde;
    };
    struct {
        VL_OUT(address_out,20,1);
        IData/*19:0*/ agnus_bitplanedma__DOT__newpt;
        IData/*19:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17;
        IData/*31:0*/ __VactIterCount;
        VlUnpacked<CData/*5:0*/, 2> agnus_bitplanedma__DOT__bplcon0_delay;
        VlUnpacked<CData/*4:0*/, 8> agnus_bitplanedma__DOT__bplpth;
        VlUnpacked<SData/*14:0*/, 8> agnus_bitplanedma__DOT__bplptl;
        VlUnpacked<CData/*0:0*/, 3> __Vm_traceActivity;
    };
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_bitplanedma__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_bitplanedma___024root(Vagnus_bitplanedma__Syms* symsp, const char* v__name);
    ~Vagnus_bitplanedma___024root();
    VL_UNCOPYABLE(Vagnus_bitplanedma___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
