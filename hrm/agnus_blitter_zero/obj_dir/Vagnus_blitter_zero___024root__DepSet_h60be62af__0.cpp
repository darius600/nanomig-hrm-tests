// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_blitter_zero.h for the primary calling header

#include "Vagnus_blitter_zero__pch.h"
#include "Vagnus_blitter_zero___024root.h"

void Vagnus_blitter_zero___024root___eval_act(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval_act\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_blitter_zero___024root___nba_sequent__TOP__0(Vagnus_blitter_zero___024root* vlSelf);

void Vagnus_blitter_zero___024root___eval_nba(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval_nba\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus_blitter_zero___024root___nba_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_blitter_zero___024root___nba_sequent__TOP__0(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___nba_sequent__TOP__0\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.clk7_en) {
        if (((IData)(vlSelfRef.enable) & (IData)(vlSelfRef.init))) {
            vlSelfRef.zero = 1U;
        } else if (((IData)(vlSelfRef.store_result) 
                    & (0U != (IData)(vlSelfRef.fill_out)))) {
            vlSelfRef.zero = 0U;
        }
    }
}

void Vagnus_blitter_zero___024root___eval_triggers__act(Vagnus_blitter_zero___024root* vlSelf);

bool Vagnus_blitter_zero___024root___eval_phase__act(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval_phase__act\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus_blitter_zero___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus_blitter_zero___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus_blitter_zero___024root___eval_phase__nba(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval_phase__nba\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus_blitter_zero___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_zero___024root___dump_triggers__nba(Vagnus_blitter_zero___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_zero___024root___dump_triggers__act(Vagnus_blitter_zero___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_blitter_zero___024root___eval(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VnbaIterCount;
    CData/*0:0*/ __VnbaContinue;
    // Body
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_blitter_zero___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("agnus_blitter_zero.v", 1, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus_blitter_zero___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("agnus_blitter_zero.v", 1, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus_blitter_zero___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus_blitter_zero___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus_blitter_zero___024root___eval_debug_assertions(Vagnus_blitter_zero___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_zero___024root___eval_debug_assertions\n"); );
    Vagnus_blitter_zero__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.clk7_en & 0xfeU)))) {
        Verilated::overWidthError("clk7_en");}
    if (VL_UNLIKELY(((vlSelfRef.enable & 0xfeU)))) {
        Verilated::overWidthError("enable");}
    if (VL_UNLIKELY(((vlSelfRef.init & 0xfeU)))) {
        Verilated::overWidthError("init");}
    if (VL_UNLIKELY(((vlSelfRef.store_result & 0xfeU)))) {
        Verilated::overWidthError("store_result");}
}
#endif  // VL_DEBUG
