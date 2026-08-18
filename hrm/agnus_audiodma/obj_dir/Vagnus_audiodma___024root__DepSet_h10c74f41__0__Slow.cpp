// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_audiodma.h for the primary calling header

#include "Vagnus_audiodma__pch.h"
#include "Vagnus_audiodma___024root.h"

VL_ATTR_COLD void Vagnus_audiodma___024root___eval_static(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_static\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
}

VL_ATTR_COLD void Vagnus_audiodma___024root___eval_initial(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_initial\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus_audiodma___024root___eval_final(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_final\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_audiodma___024root___dump_triggers__stl(Vagnus_audiodma___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vagnus_audiodma___024root___eval_phase__stl(Vagnus_audiodma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_audiodma___024root___eval_settle(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_settle\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_audiodma___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_audiodma.v", 37, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vagnus_audiodma___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_audiodma___024root___dump_triggers__stl(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___dump_triggers__stl\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

void Vagnus_audiodma___024root___ico_sequent__TOP__0(Vagnus_audiodma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_audiodma___024root___eval_stl(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_stl\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vagnus_audiodma___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD void Vagnus_audiodma___024root___eval_triggers__stl(Vagnus_audiodma___024root* vlSelf);

VL_ATTR_COLD bool Vagnus_audiodma___024root___eval_phase__stl(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_phase__stl\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vagnus_audiodma___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vagnus_audiodma___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_audiodma___024root___dump_triggers__ico(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___dump_triggers__ico\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vagnus_audiodma___024root___dump_triggers__act(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___dump_triggers__act\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vagnus_audiodma___024root___dump_triggers__nba(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___dump_triggers__nba\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

VL_ATTR_COLD void Vagnus_audiodma___024root___ctor_var_reset(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___ctor_var_reset\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->clk7_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1674876599471182170ull);
    vlSelf->dma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16670614540310729344ull);
    vlSelf->audio_dmal = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13618173280352938351ull);
    vlSelf->audio_dmas = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5976784137687008699ull);
    vlSelf->hpos = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2050750607729723554ull);
    vlSelf->reg_address_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13159730603054898824ull);
    vlSelf->reg_address_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9386968039567798889ull);
    vlSelf->data_in = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10574596302020702150ull);
    vlSelf->address_out = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 16520239430411259709ull);
    vlSelf->agnus_audiodma__DOT__audlcena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2083884830716135750ull);
    vlSelf->agnus_audiodma__DOT__audlcsel = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 2266703432771437381ull);
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->agnus_audiodma__DOT__audlch[__Vi0] = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12936548216864454831ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->agnus_audiodma__DOT__audlcl[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 10322318840599312987ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->agnus_audiodma__DOT__audpt[__Vi0] = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 7215893643937479493ull);
    }
    vlSelf->agnus_audiodma__DOT__audptout = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 10629749204721463112ull);
    vlSelf->agnus_audiodma__DOT__channel = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 14756862347713756579ull);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9526919608049418986ull);
}
