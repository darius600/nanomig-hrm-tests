// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_spritedma.h for the primary calling header

#include "Vagnus_spritedma__pch.h"
#include "Vagnus_spritedma___024root.h"

VL_ATTR_COLD void Vagnus_spritedma___024root___eval_static(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_static\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
}

VL_ATTR_COLD void Vagnus_spritedma___024root___eval_initial(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_initial\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus_spritedma___024root___eval_final(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_final\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_spritedma___024root___dump_triggers__stl(Vagnus_spritedma___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vagnus_spritedma___024root___eval_phase__stl(Vagnus_spritedma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_spritedma___024root___eval_settle(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_settle\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_spritedma___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_spritedma.v", 86, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vagnus_spritedma___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_spritedma___024root___dump_triggers__stl(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___dump_triggers__stl\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

VL_ATTR_COLD void Vagnus_spritedma___024root___stl_sequent__TOP__0(Vagnus_spritedma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_spritedma___024root___eval_stl(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_stl\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vagnus_spritedma___024root___stl_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD void Vagnus_spritedma___024root___stl_sequent__TOP__0(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___stl_sequent__TOP__0\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.ackdma) {
        vlSelfRef.agnus_spritedma__DOT__ptsel = (7U 
                                                 & (IData)(vlSelfRef.agnus_spritedma__DOT__sprite));
        vlSelfRef.agnus_spritedma__DOT__pcsel = (7U 
                                                 & (IData)(vlSelfRef.agnus_spritedma__DOT__sprite));
    } else {
        vlSelfRef.agnus_spritedma__DOT__ptsel = (7U 
                                                 & ((IData)(vlSelfRef.reg_address_in) 
                                                    >> 1U));
        vlSelfRef.agnus_spritedma__DOT__pcsel = (7U 
                                                 & ((IData)(vlSelfRef.reg_address_in) 
                                                    >> 2U));
    }
    vlSelfRef.reqdma = (((IData)(vlSelfRef.agnus_spritedma__DOT__enable) 
                         & (1U == (3U & (IData)(vlSelfRef.hpos)))) 
                        && (((IData)(vlSelfRef.vblend) 
                             | ((IData)(vlSelfRef.agnus_spritedma__DOT__sprvstop) 
                                & (~ (IData)(vlSelfRef.vbl)))) 
                            || (IData)(vlSelfRef.agnus_spritedma__DOT__sprdmastate)));
    vlSelfRef.reg_address_out = (((IData)(vlSelfRef.agnus_spritedma__DOT__enable) 
                                  & (1U == (3U & (IData)(vlSelfRef.hpos))))
                                  ? (((IData)(vlSelfRef.vblend) 
                                      | ((IData)(vlSelfRef.agnus_spritedma__DOT__sprvstop) 
                                         & (~ (IData)(vlSelfRef.vbl))))
                                      ? ((4U & (IData)(vlSelfRef.hpos))
                                          ? (0xa0U 
                                             | ((IData)(vlSelfRef.agnus_spritedma__DOT__sprite) 
                                                << 2U))
                                          : (0xa1U 
                                             | ((IData)(vlSelfRef.agnus_spritedma__DOT__sprite) 
                                                << 2U)))
                                      : ((IData)(vlSelfRef.agnus_spritedma__DOT__sprdmastate)
                                          ? ((4U & (IData)(vlSelfRef.hpos))
                                              ? (0xa2U 
                                                 | ((IData)(vlSelfRef.agnus_spritedma__DOT__sprite) 
                                                    << 2U))
                                              : (0xa3U 
                                                 | ((IData)(vlSelfRef.agnus_spritedma__DOT__sprite) 
                                                    << 2U)))
                                          : 0xffU))
                                  : 0xffU);
    vlSelfRef.address_out = ((vlSelfRef.agnus_spritedma__DOT__sprpth
                              [vlSelfRef.agnus_spritedma__DOT__sprite] 
                              << 0xfU) | vlSelfRef.agnus_spritedma__DOT__sprptl
                             [vlSelfRef.agnus_spritedma__DOT__sprite]);
    vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_0 
        = (1U & (vlSelfRef.agnus_spritedma__DOT__sprctl
                 [vlSelfRef.agnus_spritedma__DOT__sprsel] 
                 >> 3U));
    vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_3 
        = ((0x100U & (vlSelfRef.agnus_spritedma__DOT__sprctl
                      [vlSelfRef.agnus_spritedma__DOT__sprsel] 
                      << 7U)) | vlSelfRef.agnus_spritedma__DOT__sprpos
           [vlSelfRef.agnus_spritedma__DOT__sprsel]);
    vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_1 
        = (1U & (vlSelfRef.agnus_spritedma__DOT__sprctl
                 [vlSelfRef.agnus_spritedma__DOT__sprsel] 
                 >> 2U));
    vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_4 
        = ((0x100U & (vlSelfRef.agnus_spritedma__DOT__sprctl
                      [vlSelfRef.agnus_spritedma__DOT__sprsel] 
                      << 8U)) | (0xffU & (vlSelfRef.agnus_spritedma__DOT__sprctl
                                          [vlSelfRef.agnus_spritedma__DOT__sprsel] 
                                          >> 4U)));
    vlSelfRef.agnus_spritedma__DOT__newptr = (0xfffffU 
                                              & (vlSelfRef.address_out 
                                                 + 
                                                 ((0U 
                                                   == 
                                                   (3U 
                                                    & ((IData)(vlSelfRef.agnus_spritedma__DOT__fmode) 
                                                       >> 2U)))
                                                   ? 1U
                                                   : 
                                                  ((3U 
                                                    == 
                                                    (3U 
                                                     & ((IData)(vlSelfRef.agnus_spritedma__DOT__fmode) 
                                                        >> 2U)))
                                                    ? 4U
                                                    : 2U))));
    vlSelfRef.agnus_spritedma__DOT__vstop = (((IData)(vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_1) 
                                              << 9U) 
                                             | (IData)(vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_4));
    vlSelfRef.agnus_spritedma__DOT__dmastate_in = (
                                                   (~ 
                                                    ((IData)(vlSelfRef.vbl) 
                                                     | (((((IData)(vlSelfRef.ecs) 
                                                           & (IData)(vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_1)) 
                                                          << 9U) 
                                                         | (IData)(vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_4)) 
                                                        == 
                                                        (0x3ffU 
                                                         & (IData)(vlSelfRef.vpos))))) 
                                                   & ((((((IData)(vlSelfRef.ecs) 
                                                          & (IData)(vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_0)) 
                                                         << 9U) 
                                                        | (IData)(vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_3)) 
                                                       == 
                                                       (0x3ffU 
                                                        & (IData)(vlSelfRef.vpos))) 
                                                      | vlSelfRef.agnus_spritedma__DOT__dmastate_mem
                                                      [vlSelfRef.agnus_spritedma__DOT__sprsel]));
}

VL_ATTR_COLD void Vagnus_spritedma___024root___eval_triggers__stl(Vagnus_spritedma___024root* vlSelf);

VL_ATTR_COLD bool Vagnus_spritedma___024root___eval_phase__stl(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_phase__stl\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vagnus_spritedma___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vagnus_spritedma___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_spritedma___024root___dump_triggers__ico(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___dump_triggers__ico\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vagnus_spritedma___024root___dump_triggers__act(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___dump_triggers__act\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vagnus_spritedma___024root___dump_triggers__nba(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___dump_triggers__nba\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

VL_ATTR_COLD void Vagnus_spritedma___024root___ctor_var_reset(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___ctor_var_reset\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->clk7_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1674876599471182170ull);
    vlSelf->reset = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9928399931838511862ull);
    vlSelf->aga = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5205082699332066466ull);
    vlSelf->ecs = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15277934814765900103ull);
    vlSelf->reqdma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3715291408256094165ull);
    vlSelf->ackdma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11778218106920099093ull);
    vlSelf->hpos = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2050750607729723554ull);
    vlSelf->vpos = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 12586444320487401337ull);
    vlSelf->vbl = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12231882693779708794ull);
    vlSelf->vblend = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8197965225159993095ull);
    vlSelf->reg_address_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13159730603054898824ull);
    vlSelf->reg_address_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9386968039567798889ull);
    vlSelf->data_in = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10574596302020702150ull);
    vlSelf->address_out = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 16520239430411259709ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus_spritedma__DOT__sprpth[__Vi0] = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2124471815938815594ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus_spritedma__DOT__sprptl[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 13615083507920657160ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus_spritedma__DOT__sprpos[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13251232204499097185ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus_spritedma__DOT__sprposh[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6055730787090337405ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus_spritedma__DOT__sprctl[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 14318491204885784093ull);
    }
    vlSelf->agnus_spritedma__DOT__vstop = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 2493851339261927734ull);
    vlSelf->agnus_spritedma__DOT__sprite = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 5911634004486800768ull);
    vlSelf->agnus_spritedma__DOT__newptr = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 1960379430669623288ull);
    vlSelf->agnus_spritedma__DOT__enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7527241703302491887ull);
    vlSelf->agnus_spritedma__DOT__sprvstop = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16835215766156687151ull);
    vlSelf->agnus_spritedma__DOT__sprdmastate = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10094871653902592869ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus_spritedma__DOT__dmastate_mem[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9988402276321881390ull);
    }
    vlSelf->agnus_spritedma__DOT__dmastate_in = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1105707498445320814ull);
    vlSelf->agnus_spritedma__DOT__sprsel = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 2948227157550629807ull);
    vlSelf->agnus_spritedma__DOT__fmode = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9526733818191193555ull);
    vlSelf->agnus_spritedma__DOT__ptsel = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 3955572528327748393ull);
    vlSelf->agnus_spritedma__DOT__pcsel = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 18208444334676755079ull);
    vlSelf->agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2081598395350597376ull);
    vlSelf->agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2258950738034508293ull);
    vlSelf->agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_3 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8248863015686368211ull);
    vlSelf->agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_4 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3635156133747342578ull);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9526919608049418986ull);
}
