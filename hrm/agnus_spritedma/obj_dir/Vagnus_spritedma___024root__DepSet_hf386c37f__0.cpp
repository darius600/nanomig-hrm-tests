// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_spritedma.h for the primary calling header

#include "Vagnus_spritedma__pch.h"
#include "Vagnus_spritedma___024root.h"

void Vagnus_spritedma___024root___ico_sequent__TOP__0(Vagnus_spritedma___024root* vlSelf);

void Vagnus_spritedma___024root___eval_ico(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_ico\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus_spritedma___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_spritedma___024root___ico_sequent__TOP__0(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___ico_sequent__TOP__0\n"); );
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

void Vagnus_spritedma___024root___eval_triggers__ico(Vagnus_spritedma___024root* vlSelf);

bool Vagnus_spritedma___024root___eval_phase__ico(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_phase__ico\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vagnus_spritedma___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vagnus_spritedma___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vagnus_spritedma___024root___eval_act(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_act\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_spritedma___024root___nba_sequent__TOP__0(Vagnus_spritedma___024root* vlSelf);

void Vagnus_spritedma___024root___eval_nba(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_nba\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus_spritedma___024root___nba_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_spritedma___024root___nba_sequent__TOP__0(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___nba_sequent__TOP__0\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*2:0*/ __Vdly__agnus_spritedma__DOT__sprsel;
    __Vdly__agnus_spritedma__DOT__sprsel = 0;
    CData/*4:0*/ __VdlyVal__agnus_spritedma__DOT__sprpth__v0;
    __VdlyVal__agnus_spritedma__DOT__sprpth__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus_spritedma__DOT__sprpth__v0;
    __VdlyDim0__agnus_spritedma__DOT__sprpth__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_spritedma__DOT__sprpth__v0;
    __VdlySet__agnus_spritedma__DOT__sprpth__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus_spritedma__DOT__sprptl__v0;
    __VdlyVal__agnus_spritedma__DOT__sprptl__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus_spritedma__DOT__sprptl__v0;
    __VdlyDim0__agnus_spritedma__DOT__sprptl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_spritedma__DOT__sprptl__v0;
    __VdlySet__agnus_spritedma__DOT__sprptl__v0 = 0;
    CData/*7:0*/ __VdlyVal__agnus_spritedma__DOT__sprpos__v0;
    __VdlyVal__agnus_spritedma__DOT__sprpos__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus_spritedma__DOT__sprpos__v0;
    __VdlyDim0__agnus_spritedma__DOT__sprpos__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_spritedma__DOT__sprpos__v0;
    __VdlySet__agnus_spritedma__DOT__sprpos__v0 = 0;
    CData/*0:0*/ __VdlyVal__agnus_spritedma__DOT__sprposh__v0;
    __VdlyVal__agnus_spritedma__DOT__sprposh__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus_spritedma__DOT__sprposh__v0;
    __VdlyDim0__agnus_spritedma__DOT__sprposh__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_spritedma__DOT__sprposh__v0;
    __VdlySet__agnus_spritedma__DOT__sprposh__v0 = 0;
    SData/*11:0*/ __VdlyVal__agnus_spritedma__DOT__sprctl__v0;
    __VdlyVal__agnus_spritedma__DOT__sprctl__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus_spritedma__DOT__sprctl__v0;
    __VdlyDim0__agnus_spritedma__DOT__sprctl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_spritedma__DOT__sprctl__v0;
    __VdlySet__agnus_spritedma__DOT__sprctl__v0 = 0;
    CData/*0:0*/ __VdlyVal__agnus_spritedma__DOT__dmastate_mem__v0;
    __VdlyVal__agnus_spritedma__DOT__dmastate_mem__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus_spritedma__DOT__dmastate_mem__v0;
    __VdlyDim0__agnus_spritedma__DOT__dmastate_mem__v0 = 0;
    // Body
    __VdlySet__agnus_spritedma__DOT__sprposh__v0 = 0U;
    __VdlySet__agnus_spritedma__DOT__sprpth__v0 = 0U;
    __VdlySet__agnus_spritedma__DOT__sprptl__v0 = 0U;
    __VdlySet__agnus_spritedma__DOT__sprpos__v0 = 0U;
    __Vdly__agnus_spritedma__DOT__sprsel = vlSelfRef.agnus_spritedma__DOT__sprsel;
    __VdlySet__agnus_spritedma__DOT__sprctl__v0 = 0U;
    __VdlyVal__agnus_spritedma__DOT__dmastate_mem__v0 
        = vlSelfRef.agnus_spritedma__DOT__dmastate_in;
    __VdlyDim0__agnus_spritedma__DOT__dmastate_mem__v0 
        = vlSelfRef.agnus_spritedma__DOT__sprsel;
    if (((1U & ((IData)(vlSelfRef.agnus_spritedma__DOT__sprsel) 
                >> 2U)) == (1U & (IData)(vlSelfRef.hpos)))) {
        __Vdly__agnus_spritedma__DOT__sprsel = (7U 
                                                & ((IData)(1U) 
                                                   + (IData)(vlSelfRef.agnus_spritedma__DOT__sprsel)));
    }
    if ((IData)(((2U == (6U & (IData)(vlSelfRef.hpos))) 
                 & ((IData)(vlSelfRef.agnus_spritedma__DOT__sprite) 
                    == (IData)(vlSelfRef.agnus_spritedma__DOT__sprsel))))) {
        vlSelfRef.agnus_spritedma__DOT__sprvstop = 
            (((0xfffffe00U & (((IData)(vlSelfRef.ecs) 
                               << 9U) & (IData)(vlSelfRef.agnus_spritedma__DOT__vstop))) 
              | (0x1ffU & (IData)(vlSelfRef.agnus_spritedma__DOT__vstop))) 
             == (0x3ffU & (IData)(vlSelfRef.vpos)));
        vlSelfRef.agnus_spritedma__DOT__sprdmastate 
            = (vlSelfRef.agnus_spritedma__DOT__dmastate_mem
               [vlSelfRef.agnus_spritedma__DOT__sprsel] 
               & (~ ((((IData)(vlSelfRef.agnus_spritedma__DOT__fmode) 
                       >> 0xfU) & vlSelfRef.agnus_spritedma__DOT__sprposh
                      [vlSelfRef.agnus_spritedma__DOT__sprsel]) 
                     & ((1U & (IData)(vlSelfRef.vpos)) 
                        != (1U & (IData)(vlSelfRef.agnus_spritedma__DOT____VdfgRegularize_h936bceaf_0_3))))));
    }
    if (vlSelfRef.clk7_en) {
        if ((IData)((0xa0U == (0xe3U & (IData)(vlSelfRef.reg_address_in))))) {
            __VdlyVal__agnus_spritedma__DOT__sprposh__v0 
                = (1U & ((IData)(vlSelfRef.data_in) 
                         >> 7U));
            __VdlyDim0__agnus_spritedma__DOT__sprposh__v0 
                = vlSelfRef.agnus_spritedma__DOT__pcsel;
            __VdlySet__agnus_spritedma__DOT__sprposh__v0 = 1U;
            __VdlyVal__agnus_spritedma__DOT__sprpos__v0 
                = (0xffU & ((IData)(vlSelfRef.data_in) 
                            >> 8U));
            __VdlyDim0__agnus_spritedma__DOT__sprpos__v0 
                = vlSelfRef.agnus_spritedma__DOT__pcsel;
            __VdlySet__agnus_spritedma__DOT__sprpos__v0 = 1U;
        }
        if (((IData)(vlSelfRef.ackdma) | (IData)((0x90U 
                                                  == 
                                                  (0xf1U 
                                                   & (IData)(vlSelfRef.reg_address_in)))))) {
            __VdlyVal__agnus_spritedma__DOT__sprpth__v0 
                = (0x1fU & ((IData)(vlSelfRef.ackdma)
                             ? (vlSelfRef.agnus_spritedma__DOT__newptr 
                                >> 0xfU) : (IData)(vlSelfRef.data_in)));
            __VdlyDim0__agnus_spritedma__DOT__sprpth__v0 
                = vlSelfRef.agnus_spritedma__DOT__ptsel;
            __VdlySet__agnus_spritedma__DOT__sprpth__v0 = 1U;
        }
        if (((IData)(vlSelfRef.ackdma) | (IData)((0x91U 
                                                  == 
                                                  (0xf1U 
                                                   & (IData)(vlSelfRef.reg_address_in)))))) {
            __VdlyVal__agnus_spritedma__DOT__sprptl__v0 
                = (0x7fffU & ((IData)(vlSelfRef.ackdma)
                               ? vlSelfRef.agnus_spritedma__DOT__newptr
                               : ((IData)(vlSelfRef.data_in) 
                                  >> 1U)));
            __VdlyDim0__agnus_spritedma__DOT__sprptl__v0 
                = vlSelfRef.agnus_spritedma__DOT__ptsel;
            __VdlySet__agnus_spritedma__DOT__sprptl__v0 = 1U;
        }
        if ((IData)((0xa1U == (0xe3U & (IData)(vlSelfRef.reg_address_in))))) {
            __VdlyVal__agnus_spritedma__DOT__sprctl__v0 
                = ((0xff0U & ((IData)(vlSelfRef.data_in) 
                              >> 4U)) | ((0xcU & ((IData)(vlSelfRef.data_in) 
                                                  >> 3U)) 
                                         | (3U & ((IData)(vlSelfRef.data_in) 
                                                  >> 1U))));
            __VdlyDim0__agnus_spritedma__DOT__sprctl__v0 
                = vlSelfRef.agnus_spritedma__DOT__pcsel;
            __VdlySet__agnus_spritedma__DOT__sprctl__v0 = 1U;
        }
        if ((IData)((0x31U == (IData)(vlSelfRef.hpos)))) {
            vlSelfRef.agnus_spritedma__DOT__enable = 1U;
        } else if ((IData)((0x71U == (IData)(vlSelfRef.hpos)))) {
            vlSelfRef.agnus_spritedma__DOT__enable = 0U;
        }
        if (vlSelfRef.reset) {
            vlSelfRef.agnus_spritedma__DOT__fmode = 0U;
        } else if (((IData)(vlSelfRef.aga) & (0xfeU 
                                              == (IData)(vlSelfRef.reg_address_in)))) {
            vlSelfRef.agnus_spritedma__DOT__fmode = vlSelfRef.data_in;
        }
        if ((1U == (7U & (IData)(vlSelfRef.hpos)))) {
            vlSelfRef.agnus_spritedma__DOT__sprite 
                = ((4U & (VL_REDXOR_8((0x30U & (IData)(vlSelfRef.hpos))) 
                          << 2U)) | ((2U & ((~ ((IData)(vlSelfRef.hpos) 
                                                >> 4U)) 
                                            << 1U)) 
                                     | (1U & ((IData)(vlSelfRef.hpos) 
                                              >> 3U))));
        }
    }
    if (__VdlySet__agnus_spritedma__DOT__sprpth__v0) {
        vlSelfRef.agnus_spritedma__DOT__sprpth[__VdlyDim0__agnus_spritedma__DOT__sprpth__v0] 
            = __VdlyVal__agnus_spritedma__DOT__sprpth__v0;
    }
    if (__VdlySet__agnus_spritedma__DOT__sprptl__v0) {
        vlSelfRef.agnus_spritedma__DOT__sprptl[__VdlyDim0__agnus_spritedma__DOT__sprptl__v0] 
            = __VdlyVal__agnus_spritedma__DOT__sprptl__v0;
    }
    if (__VdlySet__agnus_spritedma__DOT__sprpos__v0) {
        vlSelfRef.agnus_spritedma__DOT__sprpos[__VdlyDim0__agnus_spritedma__DOT__sprpos__v0] 
            = __VdlyVal__agnus_spritedma__DOT__sprpos__v0;
    }
    if (__VdlySet__agnus_spritedma__DOT__sprctl__v0) {
        vlSelfRef.agnus_spritedma__DOT__sprctl[__VdlyDim0__agnus_spritedma__DOT__sprctl__v0] 
            = __VdlyVal__agnus_spritedma__DOT__sprctl__v0;
    }
    if (__VdlySet__agnus_spritedma__DOT__sprposh__v0) {
        vlSelfRef.agnus_spritedma__DOT__sprposh[__VdlyDim0__agnus_spritedma__DOT__sprposh__v0] 
            = __VdlyVal__agnus_spritedma__DOT__sprposh__v0;
    }
    vlSelfRef.agnus_spritedma__DOT__dmastate_mem[__VdlyDim0__agnus_spritedma__DOT__dmastate_mem__v0] 
        = __VdlyVal__agnus_spritedma__DOT__dmastate_mem__v0;
    vlSelfRef.agnus_spritedma__DOT__sprsel = __Vdly__agnus_spritedma__DOT__sprsel;
    vlSelfRef.reqdma = (((IData)(vlSelfRef.agnus_spritedma__DOT__enable) 
                         & (1U == (3U & (IData)(vlSelfRef.hpos)))) 
                        && (((IData)(vlSelfRef.vblend) 
                             | ((IData)(vlSelfRef.agnus_spritedma__DOT__sprvstop) 
                                & (~ (IData)(vlSelfRef.vbl)))) 
                            || (IData)(vlSelfRef.agnus_spritedma__DOT__sprdmastate)));
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
}

void Vagnus_spritedma___024root___eval_triggers__act(Vagnus_spritedma___024root* vlSelf);

bool Vagnus_spritedma___024root___eval_phase__act(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_phase__act\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus_spritedma___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus_spritedma___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus_spritedma___024root___eval_phase__nba(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_phase__nba\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus_spritedma___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_spritedma___024root___dump_triggers__ico(Vagnus_spritedma___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_spritedma___024root___dump_triggers__nba(Vagnus_spritedma___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_spritedma___024root___dump_triggers__act(Vagnus_spritedma___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_spritedma___024root___eval(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_spritedma___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_spritedma.v", 86, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vagnus_spritedma___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_spritedma___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_spritedma.v", 86, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus_spritedma___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../../src/minimig-aga/agnus_spritedma.v", 86, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus_spritedma___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus_spritedma___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus_spritedma___024root___eval_debug_assertions(Vagnus_spritedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_spritedma___024root___eval_debug_assertions\n"); );
    Vagnus_spritedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.clk7_en & 0xfeU)))) {
        Verilated::overWidthError("clk7_en");}
    if (VL_UNLIKELY(((vlSelfRef.reset & 0xfeU)))) {
        Verilated::overWidthError("reset");}
    if (VL_UNLIKELY(((vlSelfRef.aga & 0xfeU)))) {
        Verilated::overWidthError("aga");}
    if (VL_UNLIKELY(((vlSelfRef.ecs & 0xfeU)))) {
        Verilated::overWidthError("ecs");}
    if (VL_UNLIKELY(((vlSelfRef.ackdma & 0xfeU)))) {
        Verilated::overWidthError("ackdma");}
    if (VL_UNLIKELY(((vlSelfRef.hpos & 0xfe00U)))) {
        Verilated::overWidthError("hpos");}
    if (VL_UNLIKELY(((vlSelfRef.vpos & 0xf800U)))) {
        Verilated::overWidthError("vpos");}
    if (VL_UNLIKELY(((vlSelfRef.vbl & 0xfeU)))) {
        Verilated::overWidthError("vbl");}
    if (VL_UNLIKELY(((vlSelfRef.vblend & 0xfeU)))) {
        Verilated::overWidthError("vblend");}
}
#endif  // VL_DEBUG
