// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design internal header
// See Vagnus_beamcounter.h for the primary calling header

#ifndef VERILATED_VAGNUS_BEAMCOUNTER___024ROOT_H_
#define VERILATED_VAGNUS_BEAMCOUNTER___024ROOT_H_  // guard

#include "verilated.h"


class Vagnus_beamcounter__Syms;

class alignas(VL_CACHE_LINE_BYTES) Vagnus_beamcounter___024root final : public VerilatedModule {
  public:

    // DESIGN SPECIFIC STATE
    VL_IN8(clk,0,0);
    VL_IN8(cck,0,0);
    VL_IN8(clk7_en,0,0);
    VL_IN8(reset,0,0);
    VL_IN8(ntsc,0,0);
    VL_IN8(aga,0,0);
    VL_IN8(ecs,0,0);
    VL_IN8(a1k,0,0);
    VL_IN8(reg_address_in,8,1);
    VL_OUT8(_hsync,0,0);
    VL_OUT8(_vsync,0,0);
    VL_OUT8(field1,0,0);
    VL_OUT8(lace,0,0);
    VL_OUT8(_csync,0,0);
    VL_OUT8(hblank,0,0);
    VL_OUT8(vblank,0,0);
    VL_OUT8(vbl,0,0);
    VL_OUT8(vblend,0,0);
    VL_OUT8(eol,0,0);
    VL_OUT8(eof,0,0);
    VL_OUT8(vbl_int,0,0);
    VL_OUT8(harddis_out,0,0);
    VL_OUT8(varbeamen_out,0,0);
    CData/*0:0*/ agnus_beamcounter__DOT__long_frame;
    CData/*0:0*/ agnus_beamcounter__DOT__long_line;
    CData/*0:0*/ agnus_beamcounter__DOT__ersy;
    CData/*0:0*/ agnus_beamcounter__DOT__pal;
    CData/*0:0*/ agnus_beamcounter__DOT__end_of_line;
    CData/*7:0*/ agnus_beamcounter__DOT__hpos_r;
    CData/*0:0*/ agnus_beamcounter__DOT__vpos_inc;
    CData/*0:0*/ agnus_beamcounter__DOT__last_line;
    CData/*0:0*/ agnus_beamcounter__DOT__vpos_equ_vtotal;
    CData/*0:0*/ agnus_beamcounter__DOT__extra_line;
    CData/*0:0*/ agnus_beamcounter__DOT__vser;
    CData/*0:0*/ __VstlFirstIteration;
    CData/*0:0*/ __VicoFirstIteration;
    CData/*0:0*/ __Vtrigprevexpr___TOP__clk__0;
    CData/*0:0*/ __VactContinue;
    VL_IN16(data_in,15,0);
    VL_OUT16(data_out,15,0);
    VL_OUT16(hpos,8,0);
    VL_OUT16(vpos,10,0);
    VL_OUT16(htotal_out,8,0);
    SData/*15:0*/ agnus_beamcounter__DOT__beamcon0_reg;
    SData/*8:0*/ agnus_beamcounter__DOT__htotal_reg;
    SData/*8:0*/ agnus_beamcounter__DOT__hsstrt_reg;
    SData/*8:0*/ agnus_beamcounter__DOT__hsstop_reg;
    SData/*8:0*/ agnus_beamcounter__DOT__hcenter_reg;
    SData/*8:0*/ agnus_beamcounter__DOT__hbstrt_reg;
    SData/*8:0*/ agnus_beamcounter__DOT__hbstop_reg;
    SData/*10:0*/ agnus_beamcounter__DOT__vtotal_reg;
    SData/*10:0*/ agnus_beamcounter__DOT__vsstrt_reg;
    SData/*10:0*/ agnus_beamcounter__DOT__vsstop_reg;
    SData/*10:0*/ agnus_beamcounter__DOT__vbstop_reg;
    SData/*8:0*/ agnus_beamcounter__DOT__hsstrt;
    SData/*8:0*/ agnus_beamcounter__DOT__hsstop;
    SData/*8:0*/ agnus_beamcounter__DOT__hcenter;
    SData/*8:0*/ agnus_beamcounter__DOT__hbstrt;
    SData/*8:0*/ agnus_beamcounter__DOT__hbstop;
    SData/*10:0*/ agnus_beamcounter__DOT__vsstrt;
    SData/*10:0*/ agnus_beamcounter__DOT__vsstop;
    IData/*31:0*/ __VactIterCount;
    VlTriggerVec<1> __VstlTriggered;
    VlTriggerVec<1> __VicoTriggered;
    VlTriggerVec<1> __VactTriggered;
    VlTriggerVec<1> __VnbaTriggered;

    // INTERNAL VARIABLES
    Vagnus_beamcounter__Syms* const vlSymsp;

    // CONSTRUCTORS
    Vagnus_beamcounter___024root(Vagnus_beamcounter__Syms* symsp, const char* v__name);
    ~Vagnus_beamcounter___024root();
    VL_UNCOPYABLE(Vagnus_beamcounter___024root);

    // INTERNAL METHODS
    void __Vconfigure(bool first);
};


#endif  // guard
