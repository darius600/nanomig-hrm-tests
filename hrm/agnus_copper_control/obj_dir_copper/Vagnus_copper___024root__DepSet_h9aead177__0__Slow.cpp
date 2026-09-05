// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_copper.h for the primary calling header

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper___024root.h"

VL_ATTR_COLD void Vagnus_copper___024root___eval_static(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_static\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
}

VL_ATTR_COLD void Vagnus_copper___024root___eval_initial(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_initial\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus_copper___024root___eval_final(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_final\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__stl(Vagnus_copper___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vagnus_copper___024root___eval_phase__stl(Vagnus_copper___024root* vlSelf);

VL_ATTR_COLD void Vagnus_copper___024root___eval_settle(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_settle\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_copper___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_copper.v", 58, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vagnus_copper___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__stl(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___dump_triggers__stl\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

void Vagnus_copper___024root___ico_sequent__TOP__0(Vagnus_copper___024root* vlSelf);

VL_ATTR_COLD void Vagnus_copper___024root___eval_stl(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_stl\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vagnus_copper___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD void Vagnus_copper___024root___eval_triggers__stl(Vagnus_copper___024root* vlSelf);

VL_ATTR_COLD bool Vagnus_copper___024root___eval_phase__stl(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_phase__stl\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vagnus_copper___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vagnus_copper___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__ico(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___dump_triggers__ico\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__act(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___dump_triggers__act\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__nba(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___dump_triggers__nba\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

VL_ATTR_COLD void Vagnus_copper___024root___ctor_var_reset(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___ctor_var_reset\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->clk7_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1674876599471182170ull);
    vlSelf->reset = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9928399931838511862ull);
    vlSelf->ecs = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15277934814765900103ull);
    vlSelf->reqdma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3715291408256094165ull);
    vlSelf->ackdma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11778218106920099093ull);
    vlSelf->enadma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 254979257843725267ull);
    vlSelf->sof = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7496247244052230992ull);
    vlSelf->blit_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5165799694477622898ull);
    vlSelf->vpos = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12586444320487401337ull);
    vlSelf->hpos = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2050750607729723554ull);
    vlSelf->data_in = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10574596302020702150ull);
    vlSelf->reg_address_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13159730603054898824ull);
    vlSelf->reg_address_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9386968039567798889ull);
    vlSelf->address_out = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 16520239430411259709ull);
    vlSelf->agnus_copper__DOT__cop1lch = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 7345505075665860813ull);
    vlSelf->agnus_copper__DOT__cop1lcl = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 17057232455308464765ull);
    vlSelf->agnus_copper__DOT__cop2lch = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 6811279580088460030ull);
    vlSelf->agnus_copper__DOT__cop2lcl = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 9987489480480808843ull);
    vlSelf->agnus_copper__DOT__cdang = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3408755156290679829ull);
    vlSelf->agnus_copper__DOT__ir1 = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 5874327540253223894ull);
    vlSelf->agnus_copper__DOT__ir2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 12876925297535174006ull);
    vlSelf->agnus_copper__DOT__copper_state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 6924870969755354481ull);
    vlSelf->agnus_copper__DOT__copper_next = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 7228357157428246355ull);
    vlSelf->agnus_copper__DOT__strobe1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10023696234494555007ull);
    vlSelf->agnus_copper__DOT__strobe2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12617389783788638335ull);
    vlSelf->agnus_copper__DOT__strobe = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16941036721188769859ull);
    vlSelf->agnus_copper__DOT__skip_flag = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15850652349535892089ull);
    vlSelf->agnus_copper__DOT__selins = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1058039332086914959ull);
    vlSelf->agnus_copper__DOT__selreg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1173315653422390036ull);
    vlSelf->agnus_copper__DOT__skip = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14981927935307908385ull);
    vlSelf->agnus_copper__DOT__enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5742221107053360003ull);
    vlSelf->agnus_copper__DOT__dma_ack = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14686567226706916697ull);
    vlSelf->agnus_copper__DOT__beam_match = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16106381630393861278ull);
    vlSelf->agnus_copper__DOT__beam_match_skip = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7702626390216273578ull);
    vlSelf->agnus_copper__DOT__beam_match_wait = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12362582517921150885ull);
    vlSelf->agnus_copper__DOT__bus_ena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8374844077760006788ull);
    vlSelf->agnus_copper__DOT__bus_blk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10986875183561202115ull);
    vlSelf->agnus_copper__DOT__copjmp1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1243176890245719700ull);
    vlSelf->agnus_copper__DOT__copjmp2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 877253135451857249ull);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9526919608049418986ull);
}
