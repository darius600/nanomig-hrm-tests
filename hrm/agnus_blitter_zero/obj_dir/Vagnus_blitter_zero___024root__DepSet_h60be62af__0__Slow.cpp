// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_blitter_zero.h for the primary calling header

#include "Vagnus_blitter_zero__pch.h"
#include "Vagnus_blitter_zero___024root.h"

VL_ATTR_COLD void Vagnus_blitter_zero___024root___eval_static(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval_static\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
}

VL_ATTR_COLD void Vagnus_blitter_zero___024root___eval_initial(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval_initial\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus_blitter_zero___024root___eval_final(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval_final\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus_blitter_zero___024root___eval_settle(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval_settle\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_zero___024root___dump_triggers__act(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___dump_triggers__act\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vagnus_blitter_zero___024root___dump_triggers__nba(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___dump_triggers__nba\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

VL_ATTR_COLD void Vagnus_blitter_zero___024root___ctor_var_reset(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___ctor_var_reset\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->clk7_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1674876599471182170ull);
    vlSelf->enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11030669854614834172ull);
    vlSelf->init = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4906703793241200758ull);
    vlSelf->store_result = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2596222826163904049ull);
    vlSelf->fill_out = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11098856125777737849ull);
    vlSelf->zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10977623970759875275ull);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9526919608049418986ull);
}
