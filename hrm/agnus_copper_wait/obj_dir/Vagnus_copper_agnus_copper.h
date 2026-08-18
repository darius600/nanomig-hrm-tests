// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_copper.h for the primary calling header

#ifndef VERILATED_VAGNUS_COPPER_AGNUS_COPPER_H_
#define VERILATED_VAGNUS_COPPER_AGNUS_COPPER_H_  // guard

#include "verilated.h"


class Vagnus_copper__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_copper_agnus_copper final : public VerilatedModule {
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
    VL_OUT8(copper_state_dbg,2,0);
    VL_OUT8(dma_req_dbg,0,0);
    VL_OUT8(bus_ena_dbg,0,0);
    VL_OUT8(clk_ena_dbg,0,0);
    CData/*4:0*/ __PVT__cop1lch;
    CData/*4:0*/ __PVT__cop2lch;
    CData/*0:0*/ __PVT__cdang;
    CData/*2:0*/ __PVT__copper_state;
    CData/*2:0*/ __PVT__copper_next;
    CData/*0:0*/ __PVT__strobe1;
    CData/*0:0*/ __PVT__strobe2;
    CData/*0:0*/ __PVT__strobe;
    CData/*0:0*/ __PVT__illegalreg;
    CData/*0:0*/ __PVT__skip_flag;
    CData/*0:0*/ selins;
    CData/*0:0*/ selreg;
    CData/*0:0*/ __PVT__skip;
    CData/*0:0*/ __PVT__enable;
    CData/*0:0*/ dma_req;
    CData/*0:0*/ __PVT__dma_ack;
    CData/*0:0*/ __PVT__beam_match;
    CData/*0:0*/ __PVT__beam_match_skip;
    CData/*0:0*/ __PVT__beam_match_wait;
    CData/*0:0*/ __PVT__bus_ena;
    CData/*0:0*/ __PVT__bus_blk;
    CData/*0:0*/ __PVT__copjmp1;
    CData/*0:0*/ __PVT__copjmp2;
    CData/*0:0*/ __VdfgRegularize_he23179ad_0_0;
    VL_IN16(hpos,8,0);
    VL_IN16(data_in,15,0);
    SData/*14:0*/ __PVT__cop1lcl;
    SData/*14:0*/ __PVT__cop2lcl;
    SData/*14:0*/ __PVT__ir1;
    SData/*15:0*/ __PVT__ir2;
    VL_OUT(address_out,20,1);

    // INTERNAL VARIABLES
    Vagnus_copper__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_copper_agnus_copper(Vagnus_copper__Syms* symsp, const char* v__name);
    ~Vagnus_copper_agnus_copper();
    VL_UNCOPYABLE(Vagnus_copper_agnus_copper);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
