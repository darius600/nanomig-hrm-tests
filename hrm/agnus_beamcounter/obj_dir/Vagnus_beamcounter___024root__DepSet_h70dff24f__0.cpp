// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_beamcounter.h for the primary calling header

#include "Vagnus_beamcounter__pch.h"
#include "Vagnus_beamcounter___024root.h"

void Vagnus_beamcounter___024root___ico_sequent__TOP__0(Vagnus_beamcounter___024root* vlSelf);

void Vagnus_beamcounter___024root___eval_ico(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_ico\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus_beamcounter___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_beamcounter___024root___ico_sequent__TOP__0(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___ico_sequent__TOP__0\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.hpos = (((IData)(vlSelfRef.agnus_beamcounter__DOT__hpos_r) 
                       << 1U) | (IData)(vlSelfRef.cck));
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
}

void Vagnus_beamcounter___024root___eval_triggers__ico(Vagnus_beamcounter___024root* vlSelf);

bool Vagnus_beamcounter___024root___eval_phase__ico(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_phase__ico\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vagnus_beamcounter___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vagnus_beamcounter___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vagnus_beamcounter___024root___eval_act(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_act\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_beamcounter___024root___nba_sequent__TOP__0(Vagnus_beamcounter___024root* vlSelf);

void Vagnus_beamcounter___024root___eval_nba(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_nba\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus_beamcounter___024root___nba_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_beamcounter___024root___nba_sequent__TOP__0(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___nba_sequent__TOP__0\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    SData/*10:0*/ agnus_beamcounter__DOT__vbstop;
    agnus_beamcounter__DOT__vbstop = 0;
    CData/*0:0*/ agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_0;
    agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_0 = 0;
    CData/*0:0*/ agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_1;
    agnus_beamcounter__DOT____VdfgRegularize_h90f6e462_0_1 = 0;
    CData/*7:0*/ __Vdly__agnus_beamcounter__DOT__hpos_r;
    __Vdly__agnus_beamcounter__DOT__hpos_r = 0;
    CData/*0:0*/ __Vdly__agnus_beamcounter__DOT__long_line;
    __Vdly__agnus_beamcounter__DOT__long_line = 0;
    SData/*10:0*/ __Vdly__vpos;
    __Vdly__vpos = 0;
    CData/*0:0*/ __Vdly__agnus_beamcounter__DOT__long_frame;
    __Vdly__agnus_beamcounter__DOT__long_frame = 0;
    // Body
    __Vdly__vpos = vlSelfRef.vpos;
    __Vdly__agnus_beamcounter__DOT__long_frame = vlSelfRef.agnus_beamcounter__DOT__long_frame;
    __Vdly__agnus_beamcounter__DOT__hpos_r = vlSelfRef.agnus_beamcounter__DOT__hpos_r;
    __Vdly__agnus_beamcounter__DOT__long_line = vlSelfRef.agnus_beamcounter__DOT__long_line;
    if (vlSelfRef.clk7_en) {
        if ((0x15U == (IData)(vlSelfRef.reg_address_in))) {
            __Vdly__vpos = ((0xffU & (IData)(__Vdly__vpos)) 
                            | (0x700U & ((IData)(vlSelfRef.data_in) 
                                         << 8U)));
        } else if ((0x16U == (IData)(vlSelfRef.reg_address_in))) {
            __Vdly__vpos = ((0x700U & (IData)(__Vdly__vpos)) 
                            | (0xffU & ((IData)(vlSelfRef.data_in) 
                                        >> 8U)));
        } else if (vlSelfRef.agnus_beamcounter__DOT__vpos_inc) {
            __Vdly__vpos = ((IData)(vlSelfRef.agnus_beamcounter__DOT__last_line)
                             ? 0U : (0x7ffU & ((IData)(1U) 
                                               + (IData)(vlSelfRef.vpos))));
        }
        if ((0x16U == (IData)(vlSelfRef.reg_address_in))) {
            __Vdly__agnus_beamcounter__DOT__hpos_r 
                = (0xffU & (IData)(vlSelfRef.data_in));
        } else if (vlSelfRef.agnus_beamcounter__DOT__end_of_line) {
            __Vdly__agnus_beamcounter__DOT__hpos_r = 0U;
        } else if (((IData)(vlSelfRef.cck) & ((~ (IData)(vlSelfRef.agnus_beamcounter__DOT__ersy)) 
                                              | (0U 
                                                 != (IData)(vlSelfRef.agnus_beamcounter__DOT__hpos_r))))) {
            __Vdly__agnus_beamcounter__DOT__hpos_r 
                = (0xffU & ((IData)(1U) + (IData)(vlSelfRef.agnus_beamcounter__DOT__hpos_r)));
        }
        if (vlSelfRef.agnus_beamcounter__DOT__end_of_line) {
            if (((IData)(vlSelfRef.agnus_beamcounter__DOT__pal) 
                 | (((IData)(vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg) 
                     >> 0xbU) & (IData)(vlSelfRef.varbeamen_out)))) {
                __Vdly__agnus_beamcounter__DOT__long_line = 0U;
            } else if ((1U & (~ (((IData)(vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg) 
                                  >> 0xbU) & (IData)(vlSelfRef.varbeamen_out))))) {
                __Vdly__agnus_beamcounter__DOT__long_line 
                    = (1U & (~ (IData)(vlSelfRef.agnus_beamcounter__DOT__long_line)));
            }
        }
        if (vlSelfRef.reset) {
            __Vdly__agnus_beamcounter__DOT__long_frame = 1U;
            vlSelfRef.agnus_beamcounter__DOT__hbstop_reg = 0x66U;
            vlSelfRef.agnus_beamcounter__DOT__vsstop_reg = 5U;
            vlSelfRef.agnus_beamcounter__DOT__hbstrt_reg = 0x19U;
            vlSelfRef.agnus_beamcounter__DOT__hsstop_reg = 0x46U;
            vlSelfRef.agnus_beamcounter__DOT__hsstrt_reg = 0x25U;
            vlSelfRef.agnus_beamcounter__DOT__htotal_reg = 0x1c4U;
            vlSelfRef.agnus_beamcounter__DOT__vsstrt_reg = 2U;
            vlSelfRef.agnus_beamcounter__DOT__hcenter_reg = 0x108U;
            if (vlSelfRef.agnus_beamcounter__DOT__pal) {
                vlSelfRef.agnus_beamcounter__DOT__vbstop_reg = 0x19U;
                vlSelfRef.agnus_beamcounter__DOT__vtotal_reg = 0x137U;
            } else {
                vlSelfRef.agnus_beamcounter__DOT__vbstop_reg = 0x14U;
                vlSelfRef.agnus_beamcounter__DOT__vtotal_reg = 0x105U;
            }
            vlSelfRef.lace = 0U;
            vlSelfRef.agnus_beamcounter__DOT__ersy = 0U;
            vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg 
                = (0x20U & ((~ (IData)(vlSelfRef.ntsc)) 
                            << 5U));
            vlSelfRef.agnus_beamcounter__DOT__pal = 
                (1U & (~ (IData)(vlSelfRef.ntsc)));
        } else {
            if ((0x15U == (IData)(vlSelfRef.reg_address_in))) {
                __Vdly__agnus_beamcounter__DOT__long_frame 
                    = (1U & ((IData)(vlSelfRef.data_in) 
                             >> 0xfU));
            } else if (((IData)(vlSelfRef.eof) & (IData)(vlSelfRef.lace))) {
                __Vdly__agnus_beamcounter__DOT__long_frame 
                    = (1U & (~ (IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame)));
            }
            if (((((((((0xe0U == (IData)(vlSelfRef.reg_address_in)) 
                       | (0xefU == (IData)(vlSelfRef.reg_address_in))) 
                      | (0xe1U == (IData)(vlSelfRef.reg_address_in))) 
                     | (0xf1U == (IData)(vlSelfRef.reg_address_in))) 
                    | (0xe2U == (IData)(vlSelfRef.reg_address_in))) 
                   | (0xe3U == (IData)(vlSelfRef.reg_address_in))) 
                  | (0xe4U == (IData)(vlSelfRef.reg_address_in))) 
                 | (0xf0U == (IData)(vlSelfRef.reg_address_in)))) {
                if ((0xe0U != (IData)(vlSelfRef.reg_address_in))) {
                    if ((0xefU != (IData)(vlSelfRef.reg_address_in))) {
                        if ((0xe1U != (IData)(vlSelfRef.reg_address_in))) {
                            if ((0xf1U != (IData)(vlSelfRef.reg_address_in))) {
                                if ((0xe2U != (IData)(vlSelfRef.reg_address_in))) {
                                    if ((0xe3U == (IData)(vlSelfRef.reg_address_in))) {
                                        vlSelfRef.agnus_beamcounter__DOT__hbstop_reg 
                                            = (0x1feU 
                                               & ((IData)(vlSelfRef.data_in) 
                                                  << 1U));
                                    }
                                    if ((0xe3U != (IData)(vlSelfRef.reg_address_in))) {
                                        if ((0xe4U 
                                             != (IData)(vlSelfRef.reg_address_in))) {
                                            vlSelfRef.agnus_beamcounter__DOT__vsstrt_reg 
                                                = (0x7ffU 
                                                   & (IData)(vlSelfRef.data_in));
                                        }
                                        if ((0xe4U 
                                             == (IData)(vlSelfRef.reg_address_in))) {
                                            vlSelfRef.agnus_beamcounter__DOT__vtotal_reg 
                                                = (0x7ffU 
                                                   & (IData)(vlSelfRef.data_in));
                                        }
                                    }
                                }
                                if ((0xe2U == (IData)(vlSelfRef.reg_address_in))) {
                                    vlSelfRef.agnus_beamcounter__DOT__hbstrt_reg 
                                        = (0x1feU & 
                                           ((IData)(vlSelfRef.data_in) 
                                            << 1U));
                                }
                            }
                            if ((0xf1U == (IData)(vlSelfRef.reg_address_in))) {
                                vlSelfRef.agnus_beamcounter__DOT__hcenter_reg 
                                    = (0x1feU & ((IData)(vlSelfRef.data_in) 
                                                 << 1U));
                            }
                        }
                        if ((0xe1U == (IData)(vlSelfRef.reg_address_in))) {
                            vlSelfRef.agnus_beamcounter__DOT__hsstop_reg 
                                = (0x1feU & ((IData)(vlSelfRef.data_in) 
                                             << 1U));
                        }
                    }
                    if ((0xefU == (IData)(vlSelfRef.reg_address_in))) {
                        vlSelfRef.agnus_beamcounter__DOT__hsstrt_reg 
                            = (0x1feU & ((IData)(vlSelfRef.data_in) 
                                         << 1U));
                    }
                }
                if ((0xe0U == (IData)(vlSelfRef.reg_address_in))) {
                    vlSelfRef.agnus_beamcounter__DOT__htotal_reg 
                        = (0x1feU & ((IData)(vlSelfRef.data_in) 
                                     << 1U));
                }
            }
            if ((1U & (~ ((((((((0xe0U == (IData)(vlSelfRef.reg_address_in)) 
                                | (0xefU == (IData)(vlSelfRef.reg_address_in))) 
                               | (0xe1U == (IData)(vlSelfRef.reg_address_in))) 
                              | (0xf1U == (IData)(vlSelfRef.reg_address_in))) 
                             | (0xe2U == (IData)(vlSelfRef.reg_address_in))) 
                            | (0xe3U == (IData)(vlSelfRef.reg_address_in))) 
                           | (0xe4U == (IData)(vlSelfRef.reg_address_in))) 
                          | (0xf0U == (IData)(vlSelfRef.reg_address_in)))))) {
                if ((0xe5U == (IData)(vlSelfRef.reg_address_in))) {
                    vlSelfRef.agnus_beamcounter__DOT__vsstop_reg 
                        = (0x7ffU & (IData)(vlSelfRef.data_in));
                }
                if ((0xe5U != (IData)(vlSelfRef.reg_address_in))) {
                    if ((0xe7U == (IData)(vlSelfRef.reg_address_in))) {
                        vlSelfRef.agnus_beamcounter__DOT__vbstop_reg 
                            = (0x7ffU & (IData)(vlSelfRef.data_in));
                    }
                }
            }
            if ((0x80U == (IData)(vlSelfRef.reg_address_in))) {
                vlSelfRef.lace = (1U & ((IData)(vlSelfRef.data_in) 
                                        >> 2U));
                vlSelfRef.agnus_beamcounter__DOT__ersy 
                    = (1U & ((IData)(vlSelfRef.data_in) 
                             >> 1U));
            }
            if (((0xeeU == (IData)(vlSelfRef.reg_address_in)) 
                 & (IData)(vlSelfRef.ecs))) {
                vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg 
                    = vlSelfRef.data_in;
                vlSelfRef.agnus_beamcounter__DOT__pal 
                    = (1U & ((IData)(vlSelfRef.data_in) 
                             >> 5U));
            }
        }
        if (((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__hbstrt))) {
            vlSelfRef.hblank = 1U;
        } else if (((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__hbstop))) {
            vlSelfRef.hblank = 0U;
        }
        if (((IData)(vlSelfRef.hpos) != (IData)(vlSelfRef.agnus_beamcounter__DOT__hbstrt))) {
            if (((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__hbstop))) {
                vlSelfRef.vblank = vlSelfRef.vbl;
            }
        }
        if (((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__hsstrt))) {
            vlSelfRef._hsync = 0U;
        } else if (((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__hsstop))) {
            vlSelfRef._hsync = 1U;
        }
        if (((IData)(vlSelfRef.hpos) == (0x1ffU & ((IData)(vlSelfRef.agnus_beamcounter__DOT__hsstrt) 
                                                   - 
                                                   ((IData)(vlSelfRef.agnus_beamcounter__DOT__hsstop) 
                                                    - (IData)(vlSelfRef.agnus_beamcounter__DOT__hsstrt)))))) {
            vlSelfRef.agnus_beamcounter__DOT__vser = 1U;
        } else if (((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__hsstrt))) {
            vlSelfRef.agnus_beamcounter__DOT__vser = 0U;
        }
        vlSelfRef.vbl_int = ((8U == (IData)(vlSelfRef.hpos)) 
                             & ((IData)(vlSelfRef.vpos) 
                                == ((IData)(vlSelfRef.a1k)
                                     ? 1U : 0U)));
        if ((((((IData)(vlSelfRef.vpos) == ((IData)(1U) 
                                            + (IData)(vlSelfRef.agnus_beamcounter__DOT__vsstrt))) 
               & ((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__hsstrt))) 
              & (IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame)) 
             | ((((IData)(vlSelfRef.vpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__vsstrt)) 
                 & ((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__hcenter))) 
                & (~ (IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame))))) {
            vlSelfRef._vsync = 0U;
        } else if ((((((IData)(vlSelfRef.vpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__vsstop)) 
                      & ((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__hcenter))) 
                     & (IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame)) 
                    | ((((IData)(vlSelfRef.vpos) == (IData)(vlSelfRef.agnus_beamcounter__DOT__vsstop)) 
                        & ((IData)(vlSelfRef.hpos) 
                           == (IData)(vlSelfRef.agnus_beamcounter__DOT__hsstrt))) 
                       & (~ (IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame))))) {
            vlSelfRef._vsync = 1U;
        }
        if (vlSelfRef.agnus_beamcounter__DOT__vpos_inc) {
            vlSelfRef.agnus_beamcounter__DOT__extra_line 
                = ((IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame) 
                   & (IData)(vlSelfRef.agnus_beamcounter__DOT__vpos_equ_vtotal));
        }
        vlSelfRef.agnus_beamcounter__DOT__end_of_line 
            = ((IData)(vlSelfRef.hpos) == (0x1feU & (IData)(vlSelfRef.htotal_out)));
        vlSelfRef.agnus_beamcounter__DOT__vpos_inc 
            = (2U == (IData)(vlSelfRef.hpos));
    }
    vlSelfRef.agnus_beamcounter__DOT__hpos_r = __Vdly__agnus_beamcounter__DOT__hpos_r;
    vlSelfRef.agnus_beamcounter__DOT__long_line = __Vdly__agnus_beamcounter__DOT__long_line;
    vlSelfRef.vpos = __Vdly__vpos;
    vlSelfRef.agnus_beamcounter__DOT__long_frame = __Vdly__agnus_beamcounter__DOT__long_frame;
    vlSelfRef._csync = (((IData)(vlSelfRef._hsync) 
                         & (IData)(vlSelfRef._vsync)) 
                        | (IData)(vlSelfRef.agnus_beamcounter__DOT__vser));
    vlSelfRef.field1 = (1U & (~ (IData)(vlSelfRef.agnus_beamcounter__DOT__long_frame)));
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
    vlSelfRef.hpos = (((IData)(vlSelfRef.agnus_beamcounter__DOT__hpos_r) 
                       << 1U) | (IData)(vlSelfRef.cck));
    vlSelfRef.varbeamen_out = (1U & ((IData)(vlSelfRef.agnus_beamcounter__DOT__beamcon0_reg) 
                                     >> 7U));
    vlSelfRef.eol = vlSelfRef.agnus_beamcounter__DOT__vpos_inc;
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

void Vagnus_beamcounter___024root___eval_triggers__act(Vagnus_beamcounter___024root* vlSelf);

bool Vagnus_beamcounter___024root___eval_phase__act(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_phase__act\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus_beamcounter___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus_beamcounter___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus_beamcounter___024root___eval_phase__nba(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_phase__nba\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus_beamcounter___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_beamcounter___024root___dump_triggers__ico(Vagnus_beamcounter___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_beamcounter___024root___dump_triggers__nba(Vagnus_beamcounter___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_beamcounter___024root___dump_triggers__act(Vagnus_beamcounter___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_beamcounter___024root___eval(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VicoIterCount;
    CData/*0:0*/ __VicoContinue;
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VicoIterCount = 0U;
    vlSelfRef.__VicoFirstIteration = 1U;
    __VicoContinue = 1U;
    while (__VicoContinue) {
        if (VL_UNLIKELY(((0x64U < __VicoIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_beamcounter___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_beamcounter.v", 27, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vagnus_beamcounter___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_beamcounter___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_beamcounter.v", 27, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus_beamcounter___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../../src/minimig-aga/agnus_beamcounter.v", 27, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus_beamcounter___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus_beamcounter___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus_beamcounter___024root___eval_debug_assertions(Vagnus_beamcounter___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_beamcounter___024root___eval_debug_assertions\n"); );
    Vagnus_beamcounter__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.clk7_en & 0xfeU)))) {
        Verilated::overWidthError("clk7_en");}
    if (VL_UNLIKELY(((vlSelfRef.reset & 0xfeU)))) {
        Verilated::overWidthError("reset");}
    if (VL_UNLIKELY(((vlSelfRef.cck & 0xfeU)))) {
        Verilated::overWidthError("cck");}
    if (VL_UNLIKELY(((vlSelfRef.ntsc & 0xfeU)))) {
        Verilated::overWidthError("ntsc");}
    if (VL_UNLIKELY(((vlSelfRef.aga & 0xfeU)))) {
        Verilated::overWidthError("aga");}
    if (VL_UNLIKELY(((vlSelfRef.ecs & 0xfeU)))) {
        Verilated::overWidthError("ecs");}
    if (VL_UNLIKELY(((vlSelfRef.a1k & 0xfeU)))) {
        Verilated::overWidthError("a1k");}
}
#endif  // VL_DEBUG
