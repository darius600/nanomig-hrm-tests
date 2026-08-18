// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_bitplanedma.h for the primary calling header

#include "Vagnus_bitplanedma__pch.h"
#include "Vagnus_bitplanedma___024root.h"

void Vagnus_bitplanedma___024root___ico_sequent__TOP__0(Vagnus_bitplanedma___024root* vlSelf);

void Vagnus_bitplanedma___024root___eval_ico(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_ico\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus_bitplanedma___024root___ico_sequent__TOP__0(vlSelf);
        vlSelfRef.__Vm_traceActivity[1U] = 1U;
    }
}

extern const VlUnpacked<CData/*7:0*/, 32> Vagnus_bitplanedma__ConstPool__TABLE_h7cec6eda_0;

VL_INLINE_OPT void Vagnus_bitplanedma___024root___ico_sequent__TOP__0(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___ico_sequent__TOP__0\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5 = 0;
    CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6 = 0;
    SData/*14:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16 = 0;
    CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18 = 0;
    CData/*4:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    // Body
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16 
        = ((1U & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwstrt) 
                  ^ (IData)(vlSelfRef.vpos))) ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod)
            : (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod));
    vlSelfRef.agnus_bitplanedma__DOT__shres = ((IData)(vlSelfRef.ecs) 
                                               & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                                                  >> 5U));
    if ((0x4000U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))) {
        vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan 
            = agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16;
        vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan 
            = agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    } else {
        vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan 
            = vlSelfRef.agnus_bitplanedma__DOT__bpl2mod;
        vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan 
            = vlSelfRef.agnus_bitplanedma__DOT__bpl1mod;
    }
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18 
        = ((~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
               >> 4U)) & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__shres)) 
                          & (3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7 
        = ((IData)(vlSelfRef.agnus_bitplanedma__DOT__shres) 
           & (0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11 
        = ((((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
             >> 4U) & (0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))) 
           | ((IData)(vlSelfRef.agnus_bitplanedma__DOT__shres) 
              & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12)));
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5 
        = (1U & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__shres)) 
                 & (~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                       >> 4U))));
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6 
        = (((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
            & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12)) 
           | (((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
               >> 4U) & (3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
    vlSelfRef.agnus_bitplanedma__DOT__ddfseq_match 
        = (((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18) 
            & (7U == (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq))) 
           | (((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6) 
               & (7U == (0xfU & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))) 
              | ((~ (IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18)) 
                 & ((~ (IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6)) 
                    & (7U == (7U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))))));
    vlSelfRef.agnus_bitplanedma__DOT__plane = ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                                ? (1U 
                                                   & (~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))
                                                : ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                    ? 
                                                   ((2U 
                                                     & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                                                        << 1U)) 
                                                    | (1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                                           >> 1U))))
                                                    : 
                                                   ((((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
                                                      & (0U 
                                                         == 
                                                         (3U 
                                                          & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))) 
                                                     | ((((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                                                          >> 4U) 
                                                         & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12)) 
                                                        | ((IData)(vlSelfRef.agnus_bitplanedma__DOT__shres) 
                                                           & (3U 
                                                              == 
                                                              (3U 
                                                               & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))))))
                                                     ? (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14)
                                                     : 
                                                    ((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6)
                                                      ? 
                                                     ((8U 
                                                       & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14))
                                                      : 
                                                     ((0x18U 
                                                       & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14))))));
    __Vtableidx1 = vlSelfRef.agnus_bitplanedma__DOT__plane;
    vlSelfRef.reg_address_out = Vagnus_bitplanedma__ConstPool__TABLE_h7cec6eda_0
        [__Vtableidx1];
    vlSelfRef.dma = ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfrun) 
                     & (((IData)(vlSelfRef.agnus_bitplanedma__DOT__dmaena_delayed) 
                         >> 1U) & ((IData)(vlSelfRef.hpos) 
                                   & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__plane) 
                                      < (0xfU & ((IData)(vlSelfRef.aga)
                                                  ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)
                                                  : 
                                                 ((7U 
                                                   == 
                                                   (7U 
                                                    & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))
                                                   ? 4U
                                                   : 
                                                  (7U 
                                                   & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))))))));
    vlSelfRef.address_out = ((vlSelfRef.agnus_bitplanedma__DOT__bplpth
                              [(7U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane))] 
                              << 0xfU) | vlSelfRef.agnus_bitplanedma__DOT__bplptl
                             [(7U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane))]);
    vlSelfRef.agnus_bitplanedma__DOT__bplptr_sel = 
        (7U & ((IData)(vlSelfRef.dma) ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane)
                : ((IData)(vlSelfRef.reg_address_in) 
                   >> 1U)));
    vlSelfRef.agnus_bitplanedma__DOT__newpt = (0xfffffU 
                                               & (((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                                    ? 
                                                   ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9) 
                                                    & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                                       >> 1U))
                                                    : 
                                                   ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                     ? (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9)
                                                     : (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfend)))
                                                   ? 
                                                  ((1U 
                                                    & (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane))
                                                    ? 
                                                   (vlSelfRef.address_out 
                                                    + 
                                                    (((0xf8000U 
                                                       & ((- (IData)(
                                                                     (1U 
                                                                      & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan) 
                                                                         >> 0xeU)))) 
                                                          << 0xfU)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan)) 
                                                     + vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17))
                                                    : 
                                                   (vlSelfRef.address_out 
                                                    + 
                                                    (((0xf8000U 
                                                       & ((- (IData)(
                                                                     (1U 
                                                                      & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan) 
                                                                         >> 0xeU)))) 
                                                          << 0xfU)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan)) 
                                                     + vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17)))
                                                   : 
                                                  (vlSelfRef.address_out 
                                                   + vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17)));
}

void Vagnus_bitplanedma___024root___eval_triggers__ico(Vagnus_bitplanedma___024root* vlSelf);

bool Vagnus_bitplanedma___024root___eval_phase__ico(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_phase__ico\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vagnus_bitplanedma___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vagnus_bitplanedma___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vagnus_bitplanedma___024root___eval_act(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_act\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_bitplanedma___024root___nba_sequent__TOP__0(Vagnus_bitplanedma___024root* vlSelf);

void Vagnus_bitplanedma___024root___eval_nba(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_nba\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus_bitplanedma___024root___nba_sequent__TOP__0(vlSelf);
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
    }
}

VL_INLINE_OPT void Vagnus_bitplanedma___024root___nba_sequent__TOP__0(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___nba_sequent__TOP__0\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5 = 0;
    CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6 = 0;
    SData/*14:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16 = 0;
    CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18 = 0;
    CData/*4:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    SData/*8:0*/ __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt;
    __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt = 0;
    SData/*8:0*/ __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop;
    __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop = 0;
    SData/*10:0*/ __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde;
    __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde = 0;
    CData/*1:0*/ __Vdly__agnus_bitplanedma__DOT__dmaena_delayed;
    __Vdly__agnus_bitplanedma__DOT__dmaena_delayed = 0;
    CData/*0:0*/ __Vdly__agnus_bitplanedma__DOT__ddfend;
    __Vdly__agnus_bitplanedma__DOT__ddfend = 0;
    CData/*4:0*/ __VdlyVal__agnus_bitplanedma__DOT__bplpth__v0;
    __VdlyVal__agnus_bitplanedma__DOT__bplpth__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus_bitplanedma__DOT__bplpth__v0;
    __VdlyDim0__agnus_bitplanedma__DOT__bplpth__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_bitplanedma__DOT__bplpth__v0;
    __VdlySet__agnus_bitplanedma__DOT__bplpth__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus_bitplanedma__DOT__bplptl__v0;
    __VdlyVal__agnus_bitplanedma__DOT__bplptl__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus_bitplanedma__DOT__bplptl__v0;
    __VdlyDim0__agnus_bitplanedma__DOT__bplptl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_bitplanedma__DOT__bplptl__v0;
    __VdlySet__agnus_bitplanedma__DOT__bplptl__v0 = 0;
    CData/*5:0*/ __VdlyVal__agnus_bitplanedma__DOT__bplcon0_delay__v0;
    __VdlyVal__agnus_bitplanedma__DOT__bplcon0_delay__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus_bitplanedma__DOT__bplcon0_delay__v0;
    __VdlySet__agnus_bitplanedma__DOT__bplcon0_delay__v0 = 0;
    CData/*5:0*/ __VdlyVal__agnus_bitplanedma__DOT__bplcon0_delay__v1;
    __VdlyVal__agnus_bitplanedma__DOT__bplcon0_delay__v1 = 0;
    // Body
    __Vdly__agnus_bitplanedma__DOT__dmaena_delayed 
        = vlSelfRef.agnus_bitplanedma__DOT__dmaena_delayed;
    __VdlySet__agnus_bitplanedma__DOT__bplpth__v0 = 0U;
    __VdlySet__agnus_bitplanedma__DOT__bplptl__v0 = 0U;
    __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt 
        = vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt;
    __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop 
        = vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop;
    __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde 
        = vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde;
    __VdlySet__agnus_bitplanedma__DOT__bplcon0_delay__v0 = 0U;
    __Vdly__agnus_bitplanedma__DOT__ddfend = vlSelfRef.agnus_bitplanedma__DOT__ddfend;
    if (vlSelfRef.clk7_en) {
        if ((3U == (3U & (IData)(vlSelfRef.hpos)))) {
            __Vdly__agnus_bitplanedma__DOT__dmaena_delayed 
                = ((2U & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__dmaena_delayed) 
                          << 1U)) | (IData)(vlSelfRef.dmaena));
        }
        if (((IData)(vlSelfRef.dma) | (IData)((0x70U 
                                               == (0xf1U 
                                                   & (IData)(vlSelfRef.reg_address_in)))))) {
            __VdlyVal__agnus_bitplanedma__DOT__bplpth__v0 
                = (0x1fU & ((IData)(vlSelfRef.dma) ? 
                            (vlSelfRef.agnus_bitplanedma__DOT__newpt 
                             >> 0xfU) : (IData)(vlSelfRef.data_in)));
            __VdlyDim0__agnus_bitplanedma__DOT__bplpth__v0 
                = vlSelfRef.agnus_bitplanedma__DOT__bplptr_sel;
            __VdlySet__agnus_bitplanedma__DOT__bplpth__v0 = 1U;
        }
        if (((IData)(vlSelfRef.dma) | (IData)((0x71U 
                                               == (0xf1U 
                                                   & (IData)(vlSelfRef.reg_address_in)))))) {
            __VdlyVal__agnus_bitplanedma__DOT__bplptl__v0 
                = (0x7fffU & ((IData)(vlSelfRef.dma)
                               ? vlSelfRef.agnus_bitplanedma__DOT__newpt
                               : ((IData)(vlSelfRef.data_in) 
                                  >> 1U)));
            __VdlyDim0__agnus_bitplanedma__DOT__bplptl__v0 
                = vlSelfRef.agnus_bitplanedma__DOT__bplptr_sel;
            __VdlySet__agnus_bitplanedma__DOT__bplptl__v0 = 1U;
        }
        if ((((IData)(vlSelfRef.hpos) & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq_match)) 
             & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfend))) {
            __Vdly__agnus_bitplanedma__DOT__ddfend = 0U;
        } else if ((((IData)(vlSelfRef.hpos) & (7U 
                                                == 
                                                (7U 
                                                 & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))) 
                    & (~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfena)))) {
            __Vdly__agnus_bitplanedma__DOT__ddfend = 1U;
        }
        if ((1U & (IData)(vlSelfRef.hpos))) {
            vlSelfRef.agnus_bitplanedma__DOT__ddfseq 
                = ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfrun)
                    ? (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))
                    : 0U);
            __VdlyVal__agnus_bitplanedma__DOT__bplcon0_delay__v0 
                = vlSelfRef.agnus_bitplanedma__DOT__bplcon0;
            __VdlySet__agnus_bitplanedma__DOT__bplcon0_delay__v0 = 1U;
            __VdlyVal__agnus_bitplanedma__DOT__bplcon0_delay__v1 
                = vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delay
                [0U];
            vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed 
                = vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delay
                [1U];
            if (((((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfena) 
                   & (IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwena)) 
                  & (~ ((IData)(vlSelfRef.hpos) >> 1U))) 
                 & (IData)(vlSelfRef.agnus_bitplanedma__DOT__dmaena_delayed))) {
                vlSelfRef.agnus_bitplanedma__DOT__ddfrun = 1U;
            } else if ((((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfend) 
                         | (~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwena))) 
                        & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq_match))) {
                vlSelfRef.agnus_bitplanedma__DOT__ddfrun = 0U;
            }
            vlSelfRef.agnus_bitplanedma__DOT__ddfena 
                = vlSelfRef.agnus_bitplanedma__DOT__ddfena_0;
            vlSelfRef.agnus_bitplanedma__DOT__ddfena_0 
                = (((IData)(vlSelfRef.agnus_bitplanedma__DOT__hardena) 
                    | (IData)(vlSelfRef.harddis)) & (IData)(vlSelfRef.agnus_bitplanedma__DOT__softena));
            if (vlSelfRef.agnus_bitplanedma__DOT__hard_start) {
                vlSelfRef.agnus_bitplanedma__DOT__hardena = 1U;
            } else if (vlSelfRef.agnus_bitplanedma__DOT__hard_stop) {
                vlSelfRef.agnus_bitplanedma__DOT__hardena = 0U;
            }
            if ((((IData)(vlSelfRef.agnus_bitplanedma__DOT__soft_start) 
                  & ((IData)(vlSelfRef.ecs) | ((IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwena) 
                                               & (IData)(vlSelfRef.dmaena)))) 
                 & (0x49U != (IData)(vlSelfRef.reg_address_in)))) {
                vlSelfRef.agnus_bitplanedma__DOT__softena = 1U;
            } else if (((IData)(vlSelfRef.agnus_bitplanedma__DOT__soft_stop) 
                        | ((~ (IData)(vlSelfRef.ecs)) 
                           & (IData)(vlSelfRef.agnus_bitplanedma__DOT__hard_stop)))) {
                vlSelfRef.agnus_bitplanedma__DOT__softena = 0U;
            }
            vlSelfRef.agnus_bitplanedma__DOT__hard_start 
                = (0x18U == (0xffU & ((IData)(vlSelfRef.hpos) 
                                      >> 1U)));
            vlSelfRef.agnus_bitplanedma__DOT__hard_stop 
                = (0xd8U == (0xffU & ((IData)(vlSelfRef.hpos) 
                                      >> 1U)));
            vlSelfRef.agnus_bitplanedma__DOT__soft_start 
                = ((0xffU & ((IData)(vlSelfRef.hpos) 
                             >> 1U)) == ((0xfcU & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfstrt) 
                                                   << 1U)) 
                                         | (((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfstrt) 
                                             & (IData)(vlSelfRef.ecs)) 
                                            << 1U)));
            vlSelfRef.agnus_bitplanedma__DOT__soft_stop 
                = ((0xffU & ((IData)(vlSelfRef.hpos) 
                             >> 1U)) == ((0xfcU & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfstop) 
                                                   << 1U)) 
                                         | (((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfstop) 
                                             & (IData)(vlSelfRef.ecs)) 
                                            << 1U)));
        }
        if ((0x84U == (IData)(vlSelfRef.reg_address_in))) {
            vlSelfRef.agnus_bitplanedma__DOT__bpl1mod 
                = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                              >> 1U));
        }
        if ((0x85U == (IData)(vlSelfRef.reg_address_in))) {
            vlSelfRef.agnus_bitplanedma__DOT__bpl2mod 
                = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                              >> 1U));
        }
        if (((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstrt))) {
            __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde 
                = (1U | (IData)(__Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde));
        }
        if ((1U & (((IData)(vlSelfRef.hpos) == (IData)(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstop)) 
                   | (~ (0U != (IData)(vlSelfRef.hpos)))))) {
            __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde 
                = (0x7feU & (IData)(__Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde));
        }
        __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde 
            = ((1U & (IData)(__Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde)) 
               | (0x7feU & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde) 
                            << 1U)));
        vlSelfRef.hde = (1U & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde) 
                               >> 0xaU));
        if (vlSelfRef.reset) {
            vlSelfRef.agnus_bitplanedma__DOT__fmode = 0U;
            vlSelfRef.agnus_bitplanedma__DOT__bplcon0 = 0U;
        } else {
            if (((IData)(vlSelfRef.aga) & (0xfeU == (IData)(vlSelfRef.reg_address_in)))) {
                vlSelfRef.agnus_bitplanedma__DOT__fmode 
                    = vlSelfRef.data_in;
            }
            if ((0x80U == (IData)(vlSelfRef.reg_address_in))) {
                vlSelfRef.agnus_bitplanedma__DOT__bplcon0 
                    = ((0x20U & ((IData)(vlSelfRef.data_in) 
                                 >> 1U)) | ((0x10U 
                                             & ((IData)(vlSelfRef.data_in) 
                                                >> 0xbU)) 
                                            | ((0x7ffffff8U 
                                                & (((IData)(vlSelfRef.aga) 
                                                    << 3U) 
                                                   & ((IData)(vlSelfRef.data_in) 
                                                      >> 1U))) 
                                               | (7U 
                                                  & ((IData)(vlSelfRef.data_in) 
                                                     >> 0xcU)))));
            }
        }
        if ((1U & (~ (0U != (IData)(vlSelfRef.hpos))))) {
            if ((((IData)(vlSelfRef.agnus_bitplanedma__DOT__hdiwstrt) 
                  < (IData)(vlSelfRef.agnus_bitplanedma__DOT__hdiwstop)) 
                 & ((0x1ffU & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop) 
                               - (IData)(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt))) 
                    < (0x1ffU & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__hdiwstop) 
                                 - (IData)(vlSelfRef.agnus_bitplanedma__DOT__hdiwstrt)))))) {
                __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt 
                    = vlSelfRef.agnus_bitplanedma__DOT__hdiwstrt;
                __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop 
                    = vlSelfRef.agnus_bitplanedma__DOT__hdiwstop;
            }
            if ((1U & (~ (0U != (IData)(vlSelfRef.vpos))))) {
                if (((0U != (IData)(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt)) 
                     | (0U != (IData)(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop)))) {
                    vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstrt 
                        = vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt;
                    vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstop 
                        = vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop;
                }
                __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt = 0U;
                __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop = 0U;
            }
        }
        if (((((IData)(vlSelfRef.sof) & (~ (IData)(vlSelfRef.a1k))) 
              | ((0U == (IData)(vlSelfRef.vpos)) & (IData)(vlSelfRef.a1k))) 
             | ((IData)(vlSelfRef.vpos) == (IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwstop)))) {
            vlSelfRef.agnus_bitplanedma__DOT__vdiwena = 0U;
        } else if (((IData)(vlSelfRef.vpos) == (IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwstrt))) {
            vlSelfRef.agnus_bitplanedma__DOT__vdiwena = 1U;
        }
        if ((0x47U == (IData)(vlSelfRef.reg_address_in))) {
            vlSelfRef.agnus_bitplanedma__DOT__hdiwstrt 
                = (0xffU & (IData)(vlSelfRef.data_in));
            vlSelfRef.agnus_bitplanedma__DOT__vdiwstrt 
                = (0xffU & ((IData)(vlSelfRef.data_in) 
                            >> 8U));
        } else if (((0xf2U == (IData)(vlSelfRef.reg_address_in)) 
                    & (IData)(vlSelfRef.ecs))) {
            vlSelfRef.agnus_bitplanedma__DOT__hdiwstrt 
                = ((0xffU & (IData)(vlSelfRef.agnus_bitplanedma__DOT__hdiwstrt)) 
                   | (0x100U & ((IData)(vlSelfRef.data_in) 
                                << 3U)));
            vlSelfRef.agnus_bitplanedma__DOT__vdiwstrt 
                = ((0xffU & (IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwstrt)) 
                   | (0x700U & ((IData)(vlSelfRef.data_in) 
                                << 8U)));
        }
        if ((0x48U == (IData)(vlSelfRef.reg_address_in))) {
            vlSelfRef.agnus_bitplanedma__DOT__hdiwstop 
                = (0x100U | (0xffU & (IData)(vlSelfRef.data_in)));
            vlSelfRef.agnus_bitplanedma__DOT__vdiwstop 
                = ((0x100U & ((~ ((IData)(vlSelfRef.data_in) 
                                  >> 0xfU)) << 8U)) 
                   | (0xffU & ((IData)(vlSelfRef.data_in) 
                               >> 8U)));
        } else if (((0xf2U == (IData)(vlSelfRef.reg_address_in)) 
                    & (IData)(vlSelfRef.ecs))) {
            vlSelfRef.agnus_bitplanedma__DOT__hdiwstop 
                = ((0xffU & (IData)(vlSelfRef.agnus_bitplanedma__DOT__hdiwstop)) 
                   | (0x100U & ((IData)(vlSelfRef.data_in) 
                                >> 5U)));
            vlSelfRef.agnus_bitplanedma__DOT__vdiwstop 
                = ((0xffU & (IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwstop)) 
                   | (0x700U & (IData)(vlSelfRef.data_in)));
        }
        if ((0x49U == (IData)(vlSelfRef.reg_address_in))) {
            vlSelfRef.agnus_bitplanedma__DOT__ddfstrt 
                = (0x7fU & ((IData)(vlSelfRef.data_in) 
                            >> 1U));
        }
        if ((0x4aU == (IData)(vlSelfRef.reg_address_in))) {
            vlSelfRef.agnus_bitplanedma__DOT__ddfstop 
                = (0x7fU & ((IData)(vlSelfRef.data_in) 
                            >> 1U));
        }
    }
    if (__VdlySet__agnus_bitplanedma__DOT__bplpth__v0) {
        vlSelfRef.agnus_bitplanedma__DOT__bplpth[__VdlyDim0__agnus_bitplanedma__DOT__bplpth__v0] 
            = __VdlyVal__agnus_bitplanedma__DOT__bplpth__v0;
    }
    if (__VdlySet__agnus_bitplanedma__DOT__bplptl__v0) {
        vlSelfRef.agnus_bitplanedma__DOT__bplptl[__VdlyDim0__agnus_bitplanedma__DOT__bplptl__v0] 
            = __VdlyVal__agnus_bitplanedma__DOT__bplptl__v0;
    }
    vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde 
        = __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde;
    if (__VdlySet__agnus_bitplanedma__DOT__bplcon0_delay__v0) {
        vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delay[0U] 
            = __VdlyVal__agnus_bitplanedma__DOT__bplcon0_delay__v0;
        vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delay[1U] 
            = __VdlyVal__agnus_bitplanedma__DOT__bplcon0_delay__v1;
    }
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14 
        = ((4U & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                  << 2U)) | ((2U & ((~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                        >> 1U)) << 1U)) 
                             | (1U & (~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                         >> 2U)))));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17 
        = ((3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))
            ? 4U : ((0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))
                     ? 1U : 2U));
    vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12 = 
        ((1U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))) 
         | (2U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))));
    vlSelfRef.agnus_bitplanedma__DOT__shres = ((IData)(vlSelfRef.ecs) 
                                               & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                                                  >> 5U));
    vlSelfRef.agnus_bitplanedma__DOT__dmaena_delayed 
        = __Vdly__agnus_bitplanedma__DOT__dmaena_delayed;
    vlSelfRef.agnus_bitplanedma__DOT__ddfend = __Vdly__agnus_bitplanedma__DOT__ddfend;
    vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt 
        = __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt;
    vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop 
        = __Vdly__agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18 
        = ((~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
               >> 4U)) & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__shres)) 
                          & (3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7 
        = ((IData)(vlSelfRef.agnus_bitplanedma__DOT__shres) 
           & (0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11 
        = ((((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
             >> 4U) & (0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))) 
           | ((IData)(vlSelfRef.agnus_bitplanedma__DOT__shres) 
              & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12)));
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5 
        = (1U & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__shres)) 
                 & (~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                       >> 4U))));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9 
        = ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfend) 
           & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
              >> 2U));
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6 
        = (((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
            & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12)) 
           | (((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
               >> 4U) & (3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
    vlSelfRef.agnus_bitplanedma__DOT__ddfseq_match 
        = (((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18) 
            & (7U == (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq))) 
           | (((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6) 
               & (7U == (0xfU & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))) 
              | ((~ (IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18)) 
                 & ((~ (IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6)) 
                    & (7U == (7U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))))));
    vlSelfRef.agnus_bitplanedma__DOT__plane = ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                                ? (1U 
                                                   & (~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))
                                                : ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                    ? 
                                                   ((2U 
                                                     & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                                                        << 1U)) 
                                                    | (1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                                           >> 1U))))
                                                    : 
                                                   ((((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
                                                      & (0U 
                                                         == 
                                                         (3U 
                                                          & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))) 
                                                     | ((((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                                                          >> 4U) 
                                                         & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12)) 
                                                        | ((IData)(vlSelfRef.agnus_bitplanedma__DOT__shres) 
                                                           & (3U 
                                                              == 
                                                              (3U 
                                                               & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))))))
                                                     ? (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14)
                                                     : 
                                                    ((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6)
                                                      ? 
                                                     ((8U 
                                                       & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14))
                                                      : 
                                                     ((0x18U 
                                                       & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14))))));
    __Vtableidx1 = vlSelfRef.agnus_bitplanedma__DOT__plane;
    vlSelfRef.reg_address_out = Vagnus_bitplanedma__ConstPool__TABLE_h7cec6eda_0
        [__Vtableidx1];
    vlSelfRef.dma = ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfrun) 
                     & (((IData)(vlSelfRef.agnus_bitplanedma__DOT__dmaena_delayed) 
                         >> 1U) & ((IData)(vlSelfRef.hpos) 
                                   & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__plane) 
                                      < (0xfU & ((IData)(vlSelfRef.aga)
                                                  ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)
                                                  : 
                                                 ((7U 
                                                   == 
                                                   (7U 
                                                    & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))
                                                   ? 4U
                                                   : 
                                                  (7U 
                                                   & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))))))));
    vlSelfRef.address_out = ((vlSelfRef.agnus_bitplanedma__DOT__bplpth
                              [(7U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane))] 
                              << 0xfU) | vlSelfRef.agnus_bitplanedma__DOT__bplptl
                             [(7U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane))]);
    vlSelfRef.agnus_bitplanedma__DOT__bplptr_sel = 
        (7U & ((IData)(vlSelfRef.dma) ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane)
                : ((IData)(vlSelfRef.reg_address_in) 
                   >> 1U)));
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16 
        = ((1U & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwstrt) 
                  ^ (IData)(vlSelfRef.vpos))) ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod)
            : (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod));
    if ((0x4000U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))) {
        vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan 
            = agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16;
        vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan 
            = agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    } else {
        vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan 
            = vlSelfRef.agnus_bitplanedma__DOT__bpl2mod;
        vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan 
            = vlSelfRef.agnus_bitplanedma__DOT__bpl1mod;
    }
    vlSelfRef.agnus_bitplanedma__DOT__newpt = (0xfffffU 
                                               & (((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                                    ? 
                                                   ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9) 
                                                    & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                                       >> 1U))
                                                    : 
                                                   ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                     ? (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9)
                                                     : (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfend)))
                                                   ? 
                                                  ((1U 
                                                    & (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane))
                                                    ? 
                                                   (vlSelfRef.address_out 
                                                    + 
                                                    (((0xf8000U 
                                                       & ((- (IData)(
                                                                     (1U 
                                                                      & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan) 
                                                                         >> 0xeU)))) 
                                                          << 0xfU)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan)) 
                                                     + vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17))
                                                    : 
                                                   (vlSelfRef.address_out 
                                                    + 
                                                    (((0xf8000U 
                                                       & ((- (IData)(
                                                                     (1U 
                                                                      & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan) 
                                                                         >> 0xeU)))) 
                                                          << 0xfU)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan)) 
                                                     + vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17)))
                                                   : 
                                                  (vlSelfRef.address_out 
                                                   + vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17)));
}

void Vagnus_bitplanedma___024root___eval_triggers__act(Vagnus_bitplanedma___024root* vlSelf);

bool Vagnus_bitplanedma___024root___eval_phase__act(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_phase__act\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus_bitplanedma___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus_bitplanedma___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus_bitplanedma___024root___eval_phase__nba(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_phase__nba\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus_bitplanedma___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_bitplanedma___024root___dump_triggers__ico(Vagnus_bitplanedma___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_bitplanedma___024root___dump_triggers__nba(Vagnus_bitplanedma___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_bitplanedma___024root___dump_triggers__act(Vagnus_bitplanedma___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_bitplanedma___024root___eval(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_bitplanedma___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_bitplanedma.v", 29, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vagnus_bitplanedma___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_bitplanedma___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_bitplanedma.v", 29, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus_bitplanedma___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../../src/minimig-aga/agnus_bitplanedma.v", 29, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus_bitplanedma___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus_bitplanedma___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus_bitplanedma___024root___eval_debug_assertions(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_debug_assertions\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.clk7_en & 0xfeU)))) {
        Verilated::overWidthError("clk7_en");}
    if (VL_UNLIKELY(((vlSelfRef.reset & 0xfeU)))) {
        Verilated::overWidthError("reset");}
    if (VL_UNLIKELY(((vlSelfRef.harddis & 0xfeU)))) {
        Verilated::overWidthError("harddis");}
    if (VL_UNLIKELY(((vlSelfRef.aga & 0xfeU)))) {
        Verilated::overWidthError("aga");}
    if (VL_UNLIKELY(((vlSelfRef.ecs & 0xfeU)))) {
        Verilated::overWidthError("ecs");}
    if (VL_UNLIKELY(((vlSelfRef.a1k & 0xfeU)))) {
        Verilated::overWidthError("a1k");}
    if (VL_UNLIKELY(((vlSelfRef.sof & 0xfeU)))) {
        Verilated::overWidthError("sof");}
    if (VL_UNLIKELY(((vlSelfRef.dmaena & 0xfeU)))) {
        Verilated::overWidthError("dmaena");}
    if (VL_UNLIKELY(((vlSelfRef.vpos & 0xf800U)))) {
        Verilated::overWidthError("vpos");}
    if (VL_UNLIKELY(((vlSelfRef.hpos & 0xfe00U)))) {
        Verilated::overWidthError("hpos");}
}
#endif  // VL_DEBUG
