// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_diskdma.h for the primary calling header

#include "Vagnus_diskdma__pch.h"
#include "Vagnus_diskdma___024root.h"

void Vagnus_diskdma___024root___ico_sequent__TOP__0(Vagnus_diskdma___024root* vlSelf);

void Vagnus_diskdma___024root___eval_ico(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___eval_ico\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus_diskdma___024root___ico_sequent__TOP__0(vlSelf);
    }
}

extern const VlUnpacked<CData/*0:0*/, 1024> Vagnus_diskdma__ConstPool__TABLE_h3d6e5953_0;

VL_INLINE_OPT void Vagnus_diskdma___024root___ico_sequent__TOP__0(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___ico_sequent__TOP__0\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus_diskdma__DOT__dmaslot;
    agnus_diskdma__DOT__dmaslot = 0;
    SData/*9:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    // Body
    vlSelfRef.wr = (1U & (~ (IData)(vlSelfRef.dmas)));
    vlSelfRef.reg_address_out = ((IData)(vlSelfRef.dmas)
                                  ? 0x13U : 4U);
    __Vtableidx1 = (((IData)(vlSelfRef.speed) << 9U) 
                    | (IData)(vlSelfRef.hpos));
    agnus_diskdma__DOT__dmaslot = Vagnus_diskdma__ConstPool__TABLE_h3d6e5953_0
        [__Vtableidx1];
    vlSelfRef.dma = ((IData)(vlSelfRef.dmal) & ((IData)(agnus_diskdma__DOT__dmaslot) 
                                                & (IData)(vlSelfRef.hpos)));
    vlSelfRef.agnus_diskdma__DOT__address_outnew = 
        (0xfffffU & ((IData)(vlSelfRef.dma) ? ((IData)(1U) 
                                               + vlSelfRef.address_out)
                      : ((0xf8000U & ((IData)(vlSelfRef.data_in) 
                                      << 0xfU)) | (0x7fffU 
                                                   & ((IData)(vlSelfRef.data_in) 
                                                      >> 1U)))));
}

void Vagnus_diskdma___024root___eval_triggers__ico(Vagnus_diskdma___024root* vlSelf);

bool Vagnus_diskdma___024root___eval_phase__ico(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___eval_phase__ico\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vagnus_diskdma___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vagnus_diskdma___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vagnus_diskdma___024root___eval_act(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___eval_act\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_diskdma___024root___nba_sequent__TOP__0(Vagnus_diskdma___024root* vlSelf);

void Vagnus_diskdma___024root___eval_nba(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___eval_nba\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus_diskdma___024root___nba_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_diskdma___024root___nba_sequent__TOP__0(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___nba_sequent__TOP__0\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (vlSelfRef.clk7_en) {
        if (((IData)(vlSelfRef.dma) | (0x10U == (IData)(vlSelfRef.reg_address_in)))) {
            vlSelfRef.address_out = ((0x7fffU & vlSelfRef.address_out) 
                                     | (0xf8000U & vlSelfRef.agnus_diskdma__DOT__address_outnew));
        }
        if (((IData)(vlSelfRef.dma) | (0x11U == (IData)(vlSelfRef.reg_address_in)))) {
            vlSelfRef.address_out = ((0xf8000U & vlSelfRef.address_out) 
                                     | (0x7fffU & vlSelfRef.agnus_diskdma__DOT__address_outnew));
        }
    }
    vlSelfRef.agnus_diskdma__DOT__address_outnew = 
        (0xfffffU & ((IData)(vlSelfRef.dma) ? ((IData)(1U) 
                                               + vlSelfRef.address_out)
                      : ((0xf8000U & ((IData)(vlSelfRef.data_in) 
                                      << 0xfU)) | (0x7fffU 
                                                   & ((IData)(vlSelfRef.data_in) 
                                                      >> 1U)))));
}

void Vagnus_diskdma___024root___eval_triggers__act(Vagnus_diskdma___024root* vlSelf);

bool Vagnus_diskdma___024root___eval_phase__act(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___eval_phase__act\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus_diskdma___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus_diskdma___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus_diskdma___024root___eval_phase__nba(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___eval_phase__nba\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus_diskdma___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_diskdma___024root___dump_triggers__ico(Vagnus_diskdma___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_diskdma___024root___dump_triggers__nba(Vagnus_diskdma___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_diskdma___024root___dump_triggers__act(Vagnus_diskdma___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_diskdma___024root___eval(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___eval\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_diskdma___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_diskdma.v", 5, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vagnus_diskdma___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_diskdma___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_diskdma.v", 5, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus_diskdma___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../../src/minimig-aga/agnus_diskdma.v", 5, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus_diskdma___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus_diskdma___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus_diskdma___024root___eval_debug_assertions(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___eval_debug_assertions\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.clk7_en & 0xfeU)))) {
        Verilated::overWidthError("clk7_en");}
    if (VL_UNLIKELY(((vlSelfRef.dmal & 0xfeU)))) {
        Verilated::overWidthError("dmal");}
    if (VL_UNLIKELY(((vlSelfRef.dmas & 0xfeU)))) {
        Verilated::overWidthError("dmas");}
    if (VL_UNLIKELY(((vlSelfRef.speed & 0xfeU)))) {
        Verilated::overWidthError("speed");}
    if (VL_UNLIKELY(((vlSelfRef.hpos & 0xfe00U)))) {
        Verilated::overWidthError("hpos");}
}
#endif  // VL_DEBUG
