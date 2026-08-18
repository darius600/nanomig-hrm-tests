// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_beamcounter.h for the primary calling header

#include "Vagnus_beamcounter__pch.h"
#include "Vagnus_beamcounter___024root.h"

VL_ATTR_COLD void Vagnus_beamcounter___024root___eval_static(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_static\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
}

VL_ATTR_COLD void Vagnus_beamcounter___024root___eval_initial(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_initial\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus_beamcounter___024root___eval_final(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_final\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_beamcounter___024root___dump_triggers__stl(Vagnus_beamcounter___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vagnus_beamcounter___024root___eval_phase__stl(Vagnus_beamcounter___024root* vlSelf);

VL_ATTR_COLD void Vagnus_beamcounter___024root___eval_settle(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_settle\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VstlIterCount;
    CData/*0:0*/ __VstlContinue;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    __VstlContinue = 1U;
    while (__VstlContinue) {
        if (VL_UNLIKELY(((0x64U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_beamcounter___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_beamcounter.v", 27, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vagnus_beamcounter___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_beamcounter___024root___dump_triggers__stl(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___dump_triggers__stl\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VstlTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vagnus_beamcounter___024root___stl_sequent__TOP__0(Vagnus_beamcounter___024root* vlSelf);

VL_ATTR_COLD void Vagnus_beamcounter___024root___eval_stl(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_stl\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vagnus_beamcounter___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD void Vagnus_beamcounter___024root___stl_sequent__TOP__0(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___stl_sequent__TOP__0\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    SData/*10:0*/ agnus_beamcounter__DOT__vbstop;
    agnus_beamcounter__DOT__vbstop = 0;
    CData/*0:0*/ agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_0;
    agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_0 = 0;
    CData/*0:0*/ agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_1;
    agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_1 = 0;
    // Body
    vlSelfRef.field1 = (1U & (~ (IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame)));
    vlSelfRef.eol = vlSelfRef.agnus_beamcounter__DOT__vpos_inc;
    vlSelfRef.hpos = (((IData)(vlSelfRef.agnus_beamcounter__DOT__hpos_r) 
                       << 1U) | (IData)(vlSelfRef.cck));
    vlSelfRef._csync = (((IData)(vlSelfRef._hsync) 
                         & (IData)(vlSelfRef._vsync)) 
                        | (IData)(vlSelfRef.agnus_beamcounter__DOT__vser));
    vlSelfRef.data_out = (((2U == (IData)(vlSelfRef.reg_address_in)) 
                           | (0x15U == (IData)(vlSelfRef.reg_address_in)))
                           ? (((IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame) 
                               << 0xfU) | (((IData)(vlSelfRef.ecs) 
                                            << 0xdU) 
                                           | (((IData)(vlSelfRef.ntsc) 
                                               << 0xcU) 
                                              | ((0x300U 
                                                  & ((- (IData)((IData)(vlSelfRef.aga))) 
                                                     << 8U)) 
                                                 | (((IData)(vlSelfRef.agnus_beamcounter__DOT__long_line) 
                                                     << 7U) 
                                                    | (7U 
                                                       & ((IData)(vlSelfRef.vpos) 
                                                          >> 8U)))))))
                           : (((3U == (IData)(vlSelfRef.reg_address_in)) 
                               | (0x16U == (IData)(vlSelfRef.reg_address_in)))
                               ? ((0xff00U & ((IData)(vlSelfRef.vpos) 
                                              << 8U)) 
                                  | (IData)(vlSelfRef.agnus_beamcounter__DOT__hpos_r))
                               : 0U));
    vlSelfRef.varbeamen_out = (1U & ((IData)(vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg) 
                                     >> 7U));
    if (vlSelfRef.varbeamen_out) {
        vlSelfRef.htotal_out = vlSelfRef.agnus_beamcounter__DOT__htotal_reg;
        vlSelfRef.agnus_beamcounter__DOT__hbstrt = vlSelfRef.agnus_beamcounter__DOT__hbstrt_reg;
        vlSelfRef.agnus_beamcounter__DOT__hbstop = vlSelfRef.agnus_beamcounter__DOT__hbstop_reg;
    } else {
        vlSelfRef.htotal_out = 0x1c4U;
        vlSelfRef.agnus_beamcounter__DOT__hbstrt = 0x19U;
        vlSelfRef.agnus_beamcounter__DOT__hbstop = 0x66U;
    }
    vlSelfRef.harddis_out = (1U & (IData)(((0U != (0x5000U 
                                                   & (IData)(vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg))) 
                                           | (IData)(vlSelfRef.varbeamen_out))));
    agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_1 
        = (((IData)(vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg) 
            >> 9U) & (IData)(vlSelfRef.varbeamen_out));
    agnus_beamcounter__DOT__vbstop = ((((IData)(vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg) 
                                        >> 0xcU) & (IData)(vlSelfRef.varbeamen_out))
                                       ? (IData)(vlSelfRef.agnus_beamcounter__DOT__vbstop_reg)
                                       : ((IData)(vlSelfRef.agnus_beamcounter__DOT__pal)
                                           ? 0x19U : 0x14U));
    vlSelfRef.agnus_beamcounter__DOT__vpos_equ_vtotal 
        = ((IData)(vlSelfRef.vpos) == ((IData)(vlSelfRef.varbeamen_out)
                                        ? (IData)(vlSelfRef.agnus_beamcounter__DOT__vtotal_reg)
                                        : ((IData)(vlSelfRef.agnus_beamcounter__DOT__pal)
                                            ? 0x137U
                                            : 0x105U)));
    agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_0 
        = (((IData)(vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg) 
            >> 8U) & (IData)(vlSelfRef.varbeamen_out));
    if (agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_1) {
        vlSelfRef.agnus_beamcounter__DOT__vsstrt = vlSelfRef.agnus_beamcounter__DOT__vsstrt_reg;
        vlSelfRef.agnus_beamcounter__DOT__vsstop = vlSelfRef.agnus_beamcounter__DOT__vsstop_reg;
    } else {
        vlSelfRef.agnus_beamcounter__DOT__vsstrt = 2U;
        vlSelfRef.agnus_beamcounter__DOT__vsstop = 5U;
    }
    vlSelfRef.vbl = ((IData)(vlSelfRef.vpos) <= (IData)(agnus_beamcounter__DOT__vbstop));
    vlSelfRef.vblend = ((IData)(vlSelfRef.vpos) == (IData)(agnus_beamcounter__DOT__vbstop));
    vlSelfRef.agnus_beamcounter__DOT__last_line = ((IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame)
                                                    ? (IData)(vlSelfRef.agnus_beamcounter__DOT__extra_line)
                                                    : (IData)(vlSelfRef.agnus_beamcounter__DOT__vpos_equ_vtotal));
    if (agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_0) {
        vlSelfRef.agnus_beamcounter__DOT__hsstrt = vlSelfRef.agnus_beamcounter__DOT__hsstrt_reg;
        vlSelfRef.agnus_beamcounter__DOT__hsstop = vlSelfRef.agnus_beamcounter__DOT__hsstop_reg;
        vlSelfRef.agnus_beamcounter__DOT__hcenter = vlSelfRef.agnus_beamcounter__DOT__hcenter_reg;
    } else {
        vlSelfRef.agnus_beamcounter__DOT__hsstrt = 0x25U;
        vlSelfRef.agnus_beamcounter__DOT__hsstop = 0x46U;
        vlSelfRef.agnus_beamcounter__DOT__hcenter = 0x108U;
    }
    vlSelfRef.eof = ((IData)(vlSelfRef.agnus_beamcounter__DOT__vpos_inc) 
                     & (IData)(vlSelfRef.agnus_beamcounter__DOT__last_line));
}

VL_ATTR_COLD void Vagnus_beamcounter___024root___eval_triggers__stl(Vagnus_beamcounter___024root* vlSelf);

VL_ATTR_COLD bool Vagnus_beamcounter___024root___eval_phase__stl(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_phase__stl\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vagnus_beamcounter___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vagnus_beamcounter___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_beamcounter___024root___dump_triggers__ico(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___dump_triggers__ico\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VicoTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        VL_DBG_MSGF("         'ico' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_beamcounter___024root___dump_triggers__act(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___dump_triggers__act\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VactTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_beamcounter___024root___dump_triggers__nba(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___dump_triggers__nba\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VnbaTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vagnus_beamcounter___024root___ctor_var_reset(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___ctor_var_reset\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->clk7_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1674876599471182170ull);
    vlSelf->reset = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9928399931838511862ull);
    vlSelf->cck = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16163541962503348705ull);
    vlSelf->ntsc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2976193588104925908ull);
    vlSelf->aga = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5205082699332066466ull);
    vlSelf->ecs = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15277934814765900103ull);
    vlSelf->a1k = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17705640770547906661ull);
    vlSelf->data_in = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10574596302020702150ull);
    vlSelf->data_out = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11675680895196038875ull);
    vlSelf->reg_address_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13159730603054898824ull);
    vlSelf->hpos = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2050750607729723554ull);
    vlSelf->vpos = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 12586444320487401337ull);
    vlSelf->_hsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11470752221461756747ull);
    vlSelf->_vsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10924989827455727735ull);
    vlSelf->field1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6996431917986141852ull);
    vlSelf->lace = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14091033307413749139ull);
    vlSelf->_csync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9881154449261449394ull);
    vlSelf->hblank = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6030576639516052719ull);
    vlSelf->vblank = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18047747227580282234ull);
    vlSelf->vbl = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12231882693779708794ull);
    vlSelf->vblend = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8197965225159993095ull);
    vlSelf->eol = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2822597813121968318ull);
    vlSelf->eof = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10193470583419097248ull);
    vlSelf->vbl_int = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10064151831238951005ull);
    vlSelf->htotal_out = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7386442930809806163ull);
    vlSelf->harddis_out = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12018427238603005146ull);
    vlSelf->varbeamen_out = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7972596560280512404ull);
    vlSelf->agnus_beamcounter__DOT__long_frame = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4595074954133775408ull);
    vlSelf->agnus_beamcounter__DOT__long_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11521566524900909685ull);
    vlSelf->agnus_beamcounter__DOT__beamcon0_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17851532029421722273ull);
    vlSelf->agnus_beamcounter__DOT__ersy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9426435035990669284ull);
    vlSelf->agnus_beamcounter__DOT__pal = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5964851053931711174ull);
    vlSelf->agnus_beamcounter__DOT__htotal_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7994125344549956540ull);
    vlSelf->agnus_beamcounter__DOT__hsstrt_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 10679395569115040683ull);
    vlSelf->agnus_beamcounter__DOT__hsstop_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 4765475256727895280ull);
    vlSelf->agnus_beamcounter__DOT__hcenter_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7901959196386454721ull);
    vlSelf->agnus_beamcounter__DOT__hbstrt_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 14740891023883752825ull);
    vlSelf->agnus_beamcounter__DOT__hbstop_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 14136259019976502446ull);
    vlSelf->agnus_beamcounter__DOT__vtotal_reg = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 2519351260461700137ull);
    vlSelf->agnus_beamcounter__DOT__vsstrt_reg = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 12129784680047794288ull);
    vlSelf->agnus_beamcounter__DOT__vsstop_reg = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 5642991946744109207ull);
    vlSelf->agnus_beamcounter__DOT__vbstop_reg = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 8263539538746682204ull);
    vlSelf->agnus_beamcounter__DOT__hsstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 16192962138556786643ull);
    vlSelf->agnus_beamcounter__DOT__hsstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3498547315268277452ull);
    vlSelf->agnus_beamcounter__DOT__hcenter = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17701677766283531850ull);
    vlSelf->agnus_beamcounter__DOT__hbstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 5324937847773618596ull);
    vlSelf->agnus_beamcounter__DOT__hbstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 192614436478739883ull);
    vlSelf->agnus_beamcounter__DOT__vsstrt = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 8204846533387542288ull);
    vlSelf->agnus_beamcounter__DOT__vsstop = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 9965324299463380625ull);
    vlSelf->agnus_beamcounter__DOT__end_of_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16673714876319844543ull);
    vlSelf->agnus_beamcounter__DOT__hpos_r = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 7749959327978466683ull);
    vlSelf->agnus_beamcounter__DOT__vpos_inc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3616467722677120623ull);
    vlSelf->agnus_beamcounter__DOT__last_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4641904537051807293ull);
    vlSelf->agnus_beamcounter__DOT__vpos_equ_vtotal = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8350488887694412284ull);
    vlSelf->agnus_beamcounter__DOT__extra_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13529123652639606262ull);
    vlSelf->agnus_beamcounter__DOT__vser = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3521413771117745742ull);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9526919608049418986ull);
}
