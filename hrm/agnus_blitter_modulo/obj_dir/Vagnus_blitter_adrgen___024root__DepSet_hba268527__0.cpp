// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_blitter_adrgen.h for the primary calling header

#include "Vagnus_blitter_adrgen__pch.h"
#include "Vagnus_blitter_adrgen___024root.h"

void Vagnus_blitter_adrgen___024root___ico_sequent__TOP__0(Vagnus_blitter_adrgen___024root* vlSelf);

void Vagnus_blitter_adrgen___024root___eval_ico(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___eval_ico\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus_blitter_adrgen___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_blitter_adrgen___024root___ico_sequent__TOP__0(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___ico_sequent__TOP__0\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*19:0*/ agnus_blitter_adrgen__DOT__newptr;
    agnus_blitter_adrgen__DOT__newptr = 0;
    IData/*19:0*/ agnus_blitter_adrgen__DOT__t_newptr;
    agnus_blitter_adrgen__DOT__t_newptr = 0;
    IData/*19:0*/ agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0;
    agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0 = 0;
    // Body
    agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0 
        = ((0xf8000U & ((- (IData)((1U & (vlSelfRef.agnus_blitter_adrgen__DOT__bltmod
                                          [vlSelfRef.modsel] 
                                          >> 0xeU)))) 
                        << 0xfU)) | vlSelfRef.agnus_blitter_adrgen__DOT__bltmod
           [vlSelfRef.modsel]);
    vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_sel 
        = ((IData)(vlSelfRef.enaptr) ? (IData)(vlSelfRef.ptrsel)
            : ((2U & ((IData)(vlSelfRef.reg_address_in) 
                      >> 2U)) | (1U & ((IData)(vlSelfRef.reg_address_in) 
                                       >> 1U))));
    vlSelfRef.address_out = ((vlSelfRef.agnus_blitter_adrgen__DOT__bltpth
                              [vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_sel] 
                              << 0xfU) | vlSelfRef.agnus_blitter_adrgen__DOT__bltptl
                             [vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_sel]);
    agnus_blitter_adrgen__DOT__t_newptr = (0xfffffU 
                                           & (((~ (IData)(vlSelfRef.decptr)) 
                                               & (IData)(vlSelfRef.incptr))
                                               ? ((IData)(1U) 
                                                  + vlSelfRef.address_out)
                                               : (vlSelfRef.address_out 
                                                  - 
                                                  ((~ (IData)(vlSelfRef.incptr)) 
                                                   & (IData)(vlSelfRef.decptr)))));
    agnus_blitter_adrgen__DOT__newptr = (0xfffffU & 
                                         (((~ (IData)(vlSelfRef.submod)) 
                                           & (IData)(vlSelfRef.addmod))
                                           ? (agnus_blitter_adrgen__DOT__t_newptr 
                                              + agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0)
                                           : (((~ (IData)(vlSelfRef.addmod)) 
                                               & (IData)(vlSelfRef.submod))
                                               ? (agnus_blitter_adrgen__DOT__t_newptr 
                                                  - agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0)
                                               : agnus_blitter_adrgen__DOT__t_newptr)));
    vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_in 
        = ((IData)(vlSelfRef.enaptr) ? agnus_blitter_adrgen__DOT__newptr
            : ((0xf8000U & ((IData)(vlSelfRef.data_in) 
                            << 0xfU)) | (0x7fffU & 
                                         ((IData)(vlSelfRef.data_in) 
                                          >> 1U))));
    vlSelfRef.sign_out = (1U & (agnus_blitter_adrgen__DOT__newptr 
                                >> 0xeU));
}

void Vagnus_blitter_adrgen___024root___eval_triggers__ico(Vagnus_blitter_adrgen___024root* vlSelf);

bool Vagnus_blitter_adrgen___024root___eval_phase__ico(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___eval_phase__ico\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vagnus_blitter_adrgen___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vagnus_blitter_adrgen___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vagnus_blitter_adrgen___024root___eval_act(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___eval_act\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_blitter_adrgen___024root___nba_sequent__TOP__0(Vagnus_blitter_adrgen___024root* vlSelf);

void Vagnus_blitter_adrgen___024root___eval_nba(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___eval_nba\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus_blitter_adrgen___024root___nba_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_blitter_adrgen___024root___nba_sequent__TOP__0(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___nba_sequent__TOP__0\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*19:0*/ agnus_blitter_adrgen__DOT__newptr;
    agnus_blitter_adrgen__DOT__newptr = 0;
    IData/*19:0*/ agnus_blitter_adrgen__DOT__t_newptr;
    agnus_blitter_adrgen__DOT__t_newptr = 0;
    IData/*19:0*/ agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0;
    agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0 = 0;
    CData/*4:0*/ __VdlyVal__agnus_blitter_adrgen__DOT__bltpth__v0;
    __VdlyVal__agnus_blitter_adrgen__DOT__bltpth__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus_blitter_adrgen__DOT__bltpth__v0;
    __VdlyDim0__agnus_blitter_adrgen__DOT__bltpth__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_blitter_adrgen__DOT__bltpth__v0;
    __VdlySet__agnus_blitter_adrgen__DOT__bltpth__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus_blitter_adrgen__DOT__bltptl__v0;
    __VdlyVal__agnus_blitter_adrgen__DOT__bltptl__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus_blitter_adrgen__DOT__bltptl__v0;
    __VdlyDim0__agnus_blitter_adrgen__DOT__bltptl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_blitter_adrgen__DOT__bltptl__v0;
    __VdlySet__agnus_blitter_adrgen__DOT__bltptl__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus_blitter_adrgen__DOT__bltmod__v0;
    __VdlyVal__agnus_blitter_adrgen__DOT__bltmod__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus_blitter_adrgen__DOT__bltmod__v0;
    __VdlyDim0__agnus_blitter_adrgen__DOT__bltmod__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_blitter_adrgen__DOT__bltmod__v0;
    __VdlySet__agnus_blitter_adrgen__DOT__bltmod__v0 = 0;
    // Body
    __VdlySet__agnus_blitter_adrgen__DOT__bltmod__v0 = 0U;
    __VdlySet__agnus_blitter_adrgen__DOT__bltpth__v0 = 0U;
    __VdlySet__agnus_blitter_adrgen__DOT__bltptl__v0 = 0U;
    if (vlSelfRef.clk7_en) {
        if ((0xcU == (0x3fU & ((IData)(vlSelfRef.reg_address_in) 
                               >> 2U)))) {
            __VdlyVal__agnus_blitter_adrgen__DOT__bltmod__v0 
                = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                              >> 1U));
            __VdlyDim0__agnus_blitter_adrgen__DOT__bltmod__v0 
                = (3U & ((IData)(vlSelfRef.enaptr) ? (IData)(vlSelfRef.modsel)
                          : (IData)(vlSelfRef.reg_address_in)));
            __VdlySet__agnus_blitter_adrgen__DOT__bltmod__v0 = 1U;
        }
        if ((((((IData)(vlSelfRef.enaptr) | (0x28U 
                                             == (IData)(vlSelfRef.reg_address_in))) 
               | (0x26U == (IData)(vlSelfRef.reg_address_in))) 
              | (0x24U == (IData)(vlSelfRef.reg_address_in))) 
             | (0x2aU == (IData)(vlSelfRef.reg_address_in)))) {
            __VdlyVal__agnus_blitter_adrgen__DOT__bltpth__v0 
                = (0x1fU & (vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_in 
                            >> 0xfU));
            __VdlyDim0__agnus_blitter_adrgen__DOT__bltpth__v0 
                = vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_sel;
            __VdlySet__agnus_blitter_adrgen__DOT__bltpth__v0 = 1U;
        }
        if ((((((IData)(vlSelfRef.enaptr) | (0x29U 
                                             == (IData)(vlSelfRef.reg_address_in))) 
               | (0x27U == (IData)(vlSelfRef.reg_address_in))) 
              | (0x25U == (IData)(vlSelfRef.reg_address_in))) 
             | (0x2bU == (IData)(vlSelfRef.reg_address_in)))) {
            __VdlyVal__agnus_blitter_adrgen__DOT__bltptl__v0 
                = (0x7fffU & vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_in);
            __VdlyDim0__agnus_blitter_adrgen__DOT__bltptl__v0 
                = vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_sel;
            __VdlySet__agnus_blitter_adrgen__DOT__bltptl__v0 = 1U;
        }
    }
    if (__VdlySet__agnus_blitter_adrgen__DOT__bltmod__v0) {
        vlSelfRef.agnus_blitter_adrgen__DOT__bltmod[__VdlyDim0__agnus_blitter_adrgen__DOT__bltmod__v0] 
            = __VdlyVal__agnus_blitter_adrgen__DOT__bltmod__v0;
    }
    if (__VdlySet__agnus_blitter_adrgen__DOT__bltpth__v0) {
        vlSelfRef.agnus_blitter_adrgen__DOT__bltpth[__VdlyDim0__agnus_blitter_adrgen__DOT__bltpth__v0] 
            = __VdlyVal__agnus_blitter_adrgen__DOT__bltpth__v0;
    }
    if (__VdlySet__agnus_blitter_adrgen__DOT__bltptl__v0) {
        vlSelfRef.agnus_blitter_adrgen__DOT__bltptl[__VdlyDim0__agnus_blitter_adrgen__DOT__bltptl__v0] 
            = __VdlyVal__agnus_blitter_adrgen__DOT__bltptl__v0;
    }
    agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0 
        = ((0xf8000U & ((- (IData)((1U & (vlSelfRef.agnus_blitter_adrgen__DOT__bltmod
                                          [vlSelfRef.modsel] 
                                          >> 0xeU)))) 
                        << 0xfU)) | vlSelfRef.agnus_blitter_adrgen__DOT__bltmod
           [vlSelfRef.modsel]);
    vlSelfRef.address_out = ((vlSelfRef.agnus_blitter_adrgen__DOT__bltpth
                              [vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_sel] 
                              << 0xfU) | vlSelfRef.agnus_blitter_adrgen__DOT__bltptl
                             [vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_sel]);
    agnus_blitter_adrgen__DOT__t_newptr = (0xfffffU 
                                           & (((~ (IData)(vlSelfRef.decptr)) 
                                               & (IData)(vlSelfRef.incptr))
                                               ? ((IData)(1U) 
                                                  + vlSelfRef.address_out)
                                               : (vlSelfRef.address_out 
                                                  - 
                                                  ((~ (IData)(vlSelfRef.incptr)) 
                                                   & (IData)(vlSelfRef.decptr)))));
    agnus_blitter_adrgen__DOT__newptr = (0xfffffU & 
                                         (((~ (IData)(vlSelfRef.submod)) 
                                           & (IData)(vlSelfRef.addmod))
                                           ? (agnus_blitter_adrgen__DOT__t_newptr 
                                              + agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0)
                                           : (((~ (IData)(vlSelfRef.addmod)) 
                                               & (IData)(vlSelfRef.submod))
                                               ? (agnus_blitter_adrgen__DOT__t_newptr 
                                                  - agnus_blitter_adrgen__DOT____VdfgRegularize_h62043dcc_0_0)
                                               : agnus_blitter_adrgen__DOT__t_newptr)));
    vlSelfRef.sign_out = (1U & (agnus_blitter_adrgen__DOT__newptr 
                                >> 0xeU));
    vlSelfRef.agnus_blitter_adrgen__DOT__bltptr_in 
        = ((IData)(vlSelfRef.enaptr) ? agnus_blitter_adrgen__DOT__newptr
            : ((0xf8000U & ((IData)(vlSelfRef.data_in) 
                            << 0xfU)) | (0x7fffU & 
                                         ((IData)(vlSelfRef.data_in) 
                                          >> 1U))));
}

void Vagnus_blitter_adrgen___024root___eval_triggers__act(Vagnus_blitter_adrgen___024root* vlSelf);

bool Vagnus_blitter_adrgen___024root___eval_phase__act(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___eval_phase__act\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus_blitter_adrgen___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus_blitter_adrgen___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus_blitter_adrgen___024root___eval_phase__nba(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___eval_phase__nba\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus_blitter_adrgen___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_adrgen___024root___dump_triggers__ico(Vagnus_blitter_adrgen___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_adrgen___024root___dump_triggers__nba(Vagnus_blitter_adrgen___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_adrgen___024root___dump_triggers__act(Vagnus_blitter_adrgen___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_blitter_adrgen___024root___eval(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___eval\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_blitter_adrgen___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_blitter_adrgen.v", 4, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vagnus_blitter_adrgen___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_blitter_adrgen___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_blitter_adrgen.v", 4, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus_blitter_adrgen___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../../src/minimig-aga/agnus_blitter_adrgen.v", 4, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus_blitter_adrgen___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus_blitter_adrgen___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus_blitter_adrgen___024root___eval_debug_assertions(Vagnus_blitter_adrgen___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_adrgen___024root___eval_debug_assertions\n"); );
    Vagnus_blitter_adrgen__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.clk7_en & 0xfeU)))) {
        Verilated::overWidthError("clk7_en");}
    if (VL_UNLIKELY(((vlSelfRef.reset & 0xfeU)))) {
        Verilated::overWidthError("reset");}
    if (VL_UNLIKELY(((vlSelfRef.ptrsel & 0xfcU)))) {
        Verilated::overWidthError("ptrsel");}
    if (VL_UNLIKELY(((vlSelfRef.modsel & 0xfcU)))) {
        Verilated::overWidthError("modsel");}
    if (VL_UNLIKELY(((vlSelfRef.enaptr & 0xfeU)))) {
        Verilated::overWidthError("enaptr");}
    if (VL_UNLIKELY(((vlSelfRef.incptr & 0xfeU)))) {
        Verilated::overWidthError("incptr");}
    if (VL_UNLIKELY(((vlSelfRef.decptr & 0xfeU)))) {
        Verilated::overWidthError("decptr");}
    if (VL_UNLIKELY(((vlSelfRef.addmod & 0xfeU)))) {
        Verilated::overWidthError("addmod");}
    if (VL_UNLIKELY(((vlSelfRef.submod & 0xfeU)))) {
        Verilated::overWidthError("submod");}
}
#endif  // VL_DEBUG
