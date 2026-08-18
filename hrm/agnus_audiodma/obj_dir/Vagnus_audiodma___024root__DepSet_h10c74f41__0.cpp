// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_audiodma.h for the primary calling header

#include "Vagnus_audiodma__pch.h"
#include "Vagnus_audiodma___024root.h"

void Vagnus_audiodma___024root___ico_sequent__TOP__0(Vagnus_audiodma___024root* vlSelf);

void Vagnus_audiodma___024root___eval_ico(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_ico\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus_audiodma___024root___ico_sequent__TOP__0(vlSelf);
    }
}

extern const VlUnpacked<CData/*1:0*/, 512> Vagnus_audiodma__ConstPool__TABLE_h4848e397_0;
extern const VlUnpacked<CData/*7:0*/, 4> Vagnus_audiodma__ConstPool__TABLE_h2dee94ce_0;

VL_INLINE_OPT void Vagnus_audiodma___024root___ico_sequent__TOP__0(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___ico_sequent__TOP__0\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    SData/*8:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    CData/*1:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    // Body
    vlSelfRef.agnus_audiodma__DOT__audlcena = (1U & (IData)(
                                                            ((0x40U 
                                                              == 
                                                              (0xc0U 
                                                               & (IData)(vlSelfRef.reg_address_in))) 
                                                             & ((~ (IData)(
                                                                           (0U 
                                                                            != 
                                                                            (6U 
                                                                             & (IData)(vlSelfRef.reg_address_in))))) 
                                                                & (((IData)(vlSelfRef.reg_address_in) 
                                                                    >> 5U) 
                                                                   ^ 
                                                                   ((IData)(vlSelfRef.reg_address_in) 
                                                                    >> 4U))))));
    vlSelfRef.agnus_audiodma__DOT__audlcsel = ((2U 
                                                & ((~ 
                                                    ((IData)(vlSelfRef.reg_address_in) 
                                                     >> 4U)) 
                                                   << 1U)) 
                                               | (1U 
                                                  & ((IData)(vlSelfRef.reg_address_in) 
                                                     >> 3U)));
    vlSelfRef.dma = (1U & ((0x25U == (IData)(vlSelfRef.hpos))
                            ? (IData)(vlSelfRef.audio_dmal)
                            : ((0x29U == (IData)(vlSelfRef.hpos))
                                ? ((IData)(vlSelfRef.audio_dmal) 
                                   >> 1U) : ((0x2dU 
                                              == (IData)(vlSelfRef.hpos))
                                              ? ((IData)(vlSelfRef.audio_dmal) 
                                                 >> 2U)
                                              : ((0x31U 
                                                  == (IData)(vlSelfRef.hpos)) 
                                                 & ((IData)(vlSelfRef.audio_dmal) 
                                                    >> 3U))))));
    __Vtableidx1 = vlSelfRef.hpos;
    vlSelfRef.agnus_audiodma__DOT__channel = Vagnus_audiodma__ConstPool__TABLE_h4848e397_0
        [__Vtableidx1];
    vlSelfRef.address_out = vlSelfRef.agnus_audiodma__DOT__audpt
        [vlSelfRef.agnus_audiodma__DOT__channel];
    vlSelfRef.agnus_audiodma__DOT__audptout = vlSelfRef.agnus_audiodma__DOT__audpt
        [vlSelfRef.agnus_audiodma__DOT__channel];
    __Vtableidx2 = vlSelfRef.agnus_audiodma__DOT__channel;
    vlSelfRef.reg_address_out = Vagnus_audiodma__ConstPool__TABLE_h2dee94ce_0
        [__Vtableidx2];
}

void Vagnus_audiodma___024root___eval_triggers__ico(Vagnus_audiodma___024root* vlSelf);

bool Vagnus_audiodma___024root___eval_phase__ico(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_phase__ico\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vagnus_audiodma___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vagnus_audiodma___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vagnus_audiodma___024root___eval_act(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_act\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_audiodma___024root___nba_sequent__TOP__0(Vagnus_audiodma___024root* vlSelf);

void Vagnus_audiodma___024root___eval_nba(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_nba\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus_audiodma___024root___nba_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_audiodma___024root___nba_sequent__TOP__0(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___nba_sequent__TOP__0\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*4:0*/ __VdlyVal__agnus_audiodma__DOT__audlch__v0;
    __VdlyVal__agnus_audiodma__DOT__audlch__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus_audiodma__DOT__audlch__v0;
    __VdlyDim0__agnus_audiodma__DOT__audlch__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_audiodma__DOT__audlch__v0;
    __VdlySet__agnus_audiodma__DOT__audlch__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus_audiodma__DOT__audlcl__v0;
    __VdlyVal__agnus_audiodma__DOT__audlcl__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus_audiodma__DOT__audlcl__v0;
    __VdlyDim0__agnus_audiodma__DOT__audlcl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_audiodma__DOT__audlcl__v0;
    __VdlySet__agnus_audiodma__DOT__audlcl__v0 = 0;
    IData/*19:0*/ __VdlyVal__agnus_audiodma__DOT__audpt__v0;
    __VdlyVal__agnus_audiodma__DOT__audpt__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus_audiodma__DOT__audpt__v0;
    __VdlyDim0__agnus_audiodma__DOT__audpt__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_audiodma__DOT__audpt__v0;
    __VdlySet__agnus_audiodma__DOT__audpt__v0 = 0;
    // Body
    __VdlySet__agnus_audiodma__DOT__audlch__v0 = 0U;
    __VdlySet__agnus_audiodma__DOT__audlcl__v0 = 0U;
    __VdlySet__agnus_audiodma__DOT__audpt__v0 = 0U;
    if (vlSelfRef.clk7_en) {
        if (((IData)(vlSelfRef.agnus_audiodma__DOT__audlcena) 
             & (~ (IData)(vlSelfRef.reg_address_in)))) {
            __VdlyVal__agnus_audiodma__DOT__audlch__v0 
                = (0x1fU & (IData)(vlSelfRef.data_in));
            __VdlyDim0__agnus_audiodma__DOT__audlch__v0 
                = vlSelfRef.agnus_audiodma__DOT__audlcsel;
            __VdlySet__agnus_audiodma__DOT__audlch__v0 = 1U;
        }
        if (((IData)(vlSelfRef.agnus_audiodma__DOT__audlcena) 
             & (IData)(vlSelfRef.reg_address_in))) {
            __VdlyVal__agnus_audiodma__DOT__audlcl__v0 
                = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                              >> 1U));
            __VdlyDim0__agnus_audiodma__DOT__audlcl__v0 
                = vlSelfRef.agnus_audiodma__DOT__audlcsel;
            __VdlySet__agnus_audiodma__DOT__audlcl__v0 = 1U;
        }
        if (vlSelfRef.dma) {
            __VdlyVal__agnus_audiodma__DOT__audpt__v0 
                = (0xfffffU & ((1U & ((0x25U == (IData)(vlSelfRef.hpos))
                                       ? (IData)(vlSelfRef.audio_dmas)
                                       : ((0x29U == (IData)(vlSelfRef.hpos))
                                           ? ((IData)(vlSelfRef.audio_dmas) 
                                              >> 1U)
                                           : ((0x2dU 
                                               == (IData)(vlSelfRef.hpos))
                                               ? ((IData)(vlSelfRef.audio_dmas) 
                                                  >> 2U)
                                               : ((0x31U 
                                                   == (IData)(vlSelfRef.hpos)) 
                                                  & ((IData)(vlSelfRef.audio_dmas) 
                                                     >> 3U))))))
                                ? ((vlSelfRef.agnus_audiodma__DOT__audlch
                                    [vlSelfRef.agnus_audiodma__DOT__channel] 
                                    << 0xfU) | vlSelfRef.agnus_audiodma__DOT__audlcl
                                   [vlSelfRef.agnus_audiodma__DOT__channel])
                                : ((IData)(1U) + vlSelfRef.agnus_audiodma__DOT__audptout)));
            __VdlyDim0__agnus_audiodma__DOT__audpt__v0 
                = vlSelfRef.agnus_audiodma__DOT__channel;
            __VdlySet__agnus_audiodma__DOT__audpt__v0 = 1U;
        }
    }
    if (__VdlySet__agnus_audiodma__DOT__audlch__v0) {
        vlSelfRef.agnus_audiodma__DOT__audlch[__VdlyDim0__agnus_audiodma__DOT__audlch__v0] 
            = __VdlyVal__agnus_audiodma__DOT__audlch__v0;
    }
    if (__VdlySet__agnus_audiodma__DOT__audlcl__v0) {
        vlSelfRef.agnus_audiodma__DOT__audlcl[__VdlyDim0__agnus_audiodma__DOT__audlcl__v0] 
            = __VdlyVal__agnus_audiodma__DOT__audlcl__v0;
    }
    if (__VdlySet__agnus_audiodma__DOT__audpt__v0) {
        vlSelfRef.agnus_audiodma__DOT__audpt[__VdlyDim0__agnus_audiodma__DOT__audpt__v0] 
            = __VdlyVal__agnus_audiodma__DOT__audpt__v0;
    }
    vlSelfRef.address_out = vlSelfRef.agnus_audiodma__DOT__audpt
        [vlSelfRef.agnus_audiodma__DOT__channel];
    vlSelfRef.agnus_audiodma__DOT__audptout = vlSelfRef.agnus_audiodma__DOT__audpt
        [vlSelfRef.agnus_audiodma__DOT__channel];
}

void Vagnus_audiodma___024root___eval_triggers__act(Vagnus_audiodma___024root* vlSelf);

bool Vagnus_audiodma___024root___eval_phase__act(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_phase__act\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus_audiodma___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus_audiodma___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus_audiodma___024root___eval_phase__nba(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_phase__nba\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus_audiodma___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_audiodma___024root___dump_triggers__ico(Vagnus_audiodma___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_audiodma___024root___dump_triggers__nba(Vagnus_audiodma___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_audiodma___024root___dump_triggers__act(Vagnus_audiodma___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_audiodma___024root___eval(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_audiodma___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_audiodma.v", 37, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vagnus_audiodma___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_audiodma___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_audiodma.v", 37, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus_audiodma___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../../src/minimig-aga/agnus_audiodma.v", 37, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus_audiodma___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus_audiodma___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus_audiodma___024root___eval_debug_assertions(Vagnus_audiodma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_audiodma___024root___eval_debug_assertions\n"); );
    Vagnus_audiodma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.clk7_en & 0xfeU)))) {
        Verilated::overWidthError("clk7_en");}
    if (VL_UNLIKELY(((vlSelfRef.audio_dmal & 0xf0U)))) {
        Verilated::overWidthError("audio_dmal");}
    if (VL_UNLIKELY(((vlSelfRef.audio_dmas & 0xf0U)))) {
        Verilated::overWidthError("audio_dmas");}
    if (VL_UNLIKELY(((vlSelfRef.hpos & 0xfe00U)))) {
        Verilated::overWidthError("hpos");}
}
#endif  // VL_DEBUG
