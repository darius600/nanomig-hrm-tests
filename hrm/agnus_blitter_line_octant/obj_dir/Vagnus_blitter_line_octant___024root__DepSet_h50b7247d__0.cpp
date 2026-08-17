// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_blitter_line_octant.h for the primary calling header

#include "Vagnus_blitter_line_octant__pch.h"
#include "Vagnus_blitter_line_octant___024root.h"

void Vagnus_blitter_line_octant___024root___ico_sequent__TOP__0(Vagnus_blitter_line_octant___024root* vlSelf);

void Vagnus_blitter_line_octant___024root___eval_ico(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_ico\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus_blitter_line_octant___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_blitter_line_octant___024root___ico_sequent__TOP__0(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___ico_sequent__TOP__0\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_6;
    agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_6 = 0;
    CData/*0:0*/ agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_7;
    agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_7 = 0;
    // Body
    agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_7 
        = (1U & ((~ ((IData)(vlSelfRef.bltcon1) >> 1U)) 
                 & (~ (IData)(vlSelfRef.sign_del))));
    agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_6 
        = (1U & ((~ (IData)(vlSelfRef.sign_del)) & 
                 ((IData)(vlSelfRef.bltcon1) >> 1U)));
    vlSelfRef.lineadd = (1U & ((IData)((0U == (5U & (IData)(vlSelfRef.bltcon1)))) 
                               | (((IData)(vlSelfRef.bltcon1) 
                                   >> 2U) & (IData)(agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_7))));
    vlSelfRef.lineinc = (((IData)((4U == (5U & (IData)(vlSelfRef.bltcon1)))) 
                          | ((~ ((IData)(vlSelfRef.bltcon1) 
                                 >> 2U)) & (IData)(agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_7))) 
                         & (0xfU == (IData)(vlSelfRef.ash)));
    vlSelfRef.linesub = (1U & ((IData)((1U == (5U & (IData)(vlSelfRef.bltcon1)))) 
                               | (((IData)(vlSelfRef.bltcon1) 
                                   >> 2U) & (IData)(agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_6))));
    vlSelfRef.linedec = (((((IData)(vlSelfRef.bltcon1) 
                            >> 2U) & (IData)(vlSelfRef.bltcon1)) 
                          | ((~ ((IData)(vlSelfRef.bltcon1) 
                                 >> 2U)) & (IData)(agnus_blitter_line_octant__DOT____VdfgRegularize_h4ed35894_0_6))) 
                         & (0U == (IData)(vlSelfRef.ash)));
}

void Vagnus_blitter_line_octant___024root___eval_triggers__ico(Vagnus_blitter_line_octant___024root* vlSelf);

bool Vagnus_blitter_line_octant___024root___eval_phase__ico(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_phase__ico\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vagnus_blitter_line_octant___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vagnus_blitter_line_octant___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vagnus_blitter_line_octant___024root___eval_act(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_act\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_blitter_line_octant___024root___eval_nba(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_nba\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_blitter_line_octant___024root___eval_triggers__act(Vagnus_blitter_line_octant___024root* vlSelf);

bool Vagnus_blitter_line_octant___024root___eval_phase__act(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_phase__act\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<0> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus_blitter_line_octant___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus_blitter_line_octant___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus_blitter_line_octant___024root___eval_phase__nba(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_phase__nba\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus_blitter_line_octant___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___dump_triggers__ico(Vagnus_blitter_line_octant___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___dump_triggers__nba(Vagnus_blitter_line_octant___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___dump_triggers__act(Vagnus_blitter_line_octant___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_blitter_line_octant___024root___eval(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_blitter_line_octant___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("agnus_blitter_line_octant.v", 1, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vagnus_blitter_line_octant___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_blitter_line_octant___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("agnus_blitter_line_octant.v", 1, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus_blitter_line_octant___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("agnus_blitter_line_octant.v", 1, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus_blitter_line_octant___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus_blitter_line_octant___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus_blitter_line_octant___024root___eval_debug_assertions(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_debug_assertions\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.bltcon1 & 0xf8U)))) {
        Verilated::overWidthError("bltcon1");}
    if (VL_UNLIKELY(((vlSelfRef.sign_del & 0xfeU)))) {
        Verilated::overWidthError("sign_del");}
    if (VL_UNLIKELY(((vlSelfRef.ash & 0xf0U)))) {
        Verilated::overWidthError("ash");}
}
#endif  // VL_DEBUG
