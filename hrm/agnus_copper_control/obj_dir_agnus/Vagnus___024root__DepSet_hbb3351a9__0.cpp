// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus.h for the primary calling header

#include "Vagnus__pch.h"
#include "Vagnus___024root.h"

void Vagnus___024root___ico_sequent__TOP__0(Vagnus___024root* vlSelf);

void Vagnus___024root___eval_ico(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_ico\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus___024root___ico_sequent__TOP__0(vlSelf);
    }
}

extern const VlUnpacked<CData/*1:0*/, 512> Vagnus__ConstPool__TABLE_h4848e397_0;
extern const VlUnpacked<CData/*0:0*/, 1024> Vagnus__ConstPool__TABLE_h3d6e5953_0;
extern const VlUnpacked<CData/*7:0*/, 4> Vagnus__ConstPool__TABLE_h2dee94ce_0;
extern const VlUnpacked<CData/*7:0*/, 32> Vagnus__ConstPool__TABLE_h7cec6eda_0;

VL_INLINE_OPT void Vagnus___024root___ico_sequent__TOP__0(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___ico_sequent__TOP__0\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus__DOT__req_spr;
    agnus__DOT__req_spr = 0;
    CData/*0:0*/ agnus__DOT__dma_spr;
    agnus__DOT__dma_spr = 0;
    CData/*0:0*/ agnus__DOT__dma_cop;
    agnus__DOT__dma_cop = 0;
    CData/*0:0*/ agnus__DOT__dma_blt;
    agnus__DOT__dma_blt = 0;
    CData/*0:0*/ agnus__DOT__dma_ref;
    agnus__DOT__dma_ref = 0;
    CData/*0:0*/ agnus__DOT__ack_cop;
    agnus__DOT__ack_cop = 0;
    CData/*0:0*/ agnus__DOT__ack_blt;
    agnus__DOT__ack_blt = 0;
    CData/*7:0*/ agnus__DOT__reg_address_aud;
    agnus__DOT__reg_address_aud = 0;
    IData/*19:0*/ agnus__DOT__address_bpl;
    agnus__DOT__address_bpl = 0;
    CData/*7:0*/ agnus__DOT__reg_address_bpl;
    agnus__DOT__reg_address_bpl = 0;
    CData/*7:0*/ agnus__DOT__reg_address_spr;
    agnus__DOT__reg_address_spr = 0;
    IData/*19:0*/ agnus__DOT__address_blt;
    agnus__DOT__address_blt = 0;
    CData/*0:0*/ agnus__DOT__we_blt;
    agnus__DOT__we_blt = 0;
    CData/*0:0*/ agnus__DOT__dsk1__DOT__dmaslot;
    agnus__DOT__dsk1__DOT__dmaslot = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT__shres;
    agnus__DOT__bpd1__DOT__shres = 0;
    CData/*4:0*/ agnus__DOT__bpd1__DOT__plane;
    agnus__DOT__bpd1__DOT__plane = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18 = 0;
    CData/*0:0*/ agnus__DOT__cp1__DOT__illegalreg;
    agnus__DOT__cp1__DOT__illegalreg = 0;
    CData/*0:0*/ agnus__DOT__cp1__DOT__dma_req;
    agnus__DOT__cp1__DOT__dma_req = 0;
    CData/*0:0*/ agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0;
    agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0 = 0;
    IData/*19:0*/ agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr;
    agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr = 0;
    SData/*9:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    SData/*8:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    CData/*1:0*/ __Vtableidx3;
    __Vtableidx3 = 0;
    CData/*4:0*/ __Vtableidx4;
    __Vtableidx4 = 0;
    // Body
    vlSelfRef.agnus__DOT__spr1__DOT__dmastate_in = 
        ((~ ((IData)(vlSelfRef.agnus__DOT__vbl) | (
                                                   ((((IData)(vlSelfRef.ecs) 
                                                      & (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_1)) 
                                                     << 9U) 
                                                    | (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_4)) 
                                                   == 
                                                   (0x3ffU 
                                                    & (IData)(vlSelfRef.agnus__DOT__vpos))))) 
         & ((((((IData)(vlSelfRef.ecs) & (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_0)) 
               << 9U) | (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_3)) 
             == (0x3ffU & (IData)(vlSelfRef.agnus__DOT__vpos))) 
            | vlSelfRef.agnus__DOT__spr1__DOT__dmastate_mem
            [vlSelfRef.agnus__DOT__spr1__DOT__sprsel]));
    vlSelfRef.__VdfgRegularize_h3c5bdd75_0_2 = ((0xf8000U 
                                                 & ((IData)(vlSelfRef.data_in) 
                                                    << 0xfU)) 
                                                | (0x7fffU 
                                                   & ((IData)(vlSelfRef.data_in) 
                                                      >> 1U)));
    agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0 
        = ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__bus_ena) 
           & (IData)(vlSelfRef.cck));
    agnus__DOT__cp1__DOT__illegalreg = (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__cdang)) 
                                         & (0U == (0x180U 
                                                   & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)))) 
                                        | ((~ (IData)(vlSelfRef.ecs)) 
                                           & (0U == 
                                              (0x1c0U 
                                               & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)))));
    vlSelfRef.agnus__DOT__hpos = (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                   << 1U) | (IData)(vlSelfRef.cck));
    agnus__DOT__bpd1__DOT__shres = ((IData)(vlSelfRef.ecs) 
                                    & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
                                       >> 5U));
    vlSelfRef.agnus__DOT__cp1__DOT__enable = ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__bus_blk)) 
                                              & (IData)(agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0));
    vlSelfRef.agnus__DOT__cp1__DOT__selreg = ((1U & 
                                               ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                >> 2U)) 
                                              && ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                       >> 1U))) 
                                                  && ((1U 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)) 
                                                      && ((1U 
                                                           & (~ 
                                                              ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                               & (IData)(agnus__DOT__cp1__DOT__illegalreg)))) 
                                                          && ((1U 
                                                               & (~ 
                                                                  ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                                   & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag)))) 
                                                              && (1U 
                                                                  & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))))))));
    vlSelfRef.agnus__DOT__cp1__DOT__selins = ((1U & 
                                               ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                >> 2U)) 
                                              && ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                       >> 1U))) 
                                                  && ((1U 
                                                       & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))) 
                                                      || ((1U 
                                                           & (~ 
                                                              ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                               & (IData)(agnus__DOT__cp1__DOT__illegalreg)))) 
                                                          && (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                               & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag)) 
                                                              || (1U 
                                                                  & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)))))));
    if ((0x25U == (IData)(vlSelfRef.agnus__DOT__hpos))) {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & (IData)(vlSelfRef.audio_dmas));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & (IData)(vlSelfRef.audio_dmal));
    } else if ((0x29U == (IData)(vlSelfRef.agnus__DOT__hpos))) {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmas) 
                                                    >> 1U));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmal) 
                                                    >> 1U));
    } else if ((0x2dU == (IData)(vlSelfRef.agnus__DOT__hpos))) {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmas) 
                                                    >> 2U));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmal) 
                                                    >> 2U));
    } else {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & ((0x31U 
                                                     == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                                                    & ((IData)(vlSelfRef.audio_dmas) 
                                                       >> 3U)));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & ((0x31U 
                                                     == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                                                    & ((IData)(vlSelfRef.audio_dmal) 
                                                       >> 3U)));
    }
    vlSelfRef.strhor_denise = ((0xbU == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                               & ((8U < (IData)(vlSelfRef.agnus__DOT__vpos)) 
                                  | (IData)(vlSelfRef.ecs)));
    agnus__DOT__reg_address_spr = (((IData)(vlSelfRef.agnus__DOT__spr1__DOT__enable) 
                                    & (1U == (3U & (IData)(vlSelfRef.agnus__DOT__hpos))))
                                    ? (((IData)(vlSelfRef.agnus__DOT__vblend) 
                                        | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprvstop) 
                                           & (~ (IData)(vlSelfRef.agnus__DOT__vbl))))
                                        ? ((4U & (IData)(vlSelfRef.agnus__DOT__hpos))
                                            ? (0xa0U 
                                               | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                  << 2U))
                                            : (0xa1U 
                                               | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                  << 2U)))
                                        : ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprdmastate)
                                            ? ((4U 
                                                & (IData)(vlSelfRef.agnus__DOT__hpos))
                                                ? (0xa2U 
                                                   | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                      << 2U))
                                                : (0xa3U 
                                                   | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                      << 2U)))
                                            : 0xffU))
                                    : 0xffU);
    __Vtableidx2 = vlSelfRef.agnus__DOT__hpos;
    vlSelfRef.agnus__DOT__aud1__DOT__channel = Vagnus__ConstPool__TABLE_h4848e397_0
        [__Vtableidx2];
    vlSelfRef.strhor_paula = (0xdU == (IData)(vlSelfRef.agnus__DOT__hpos));
    agnus__DOT__req_spr = (((IData)(vlSelfRef.agnus__DOT__spr1__DOT__enable) 
                            & (1U == (3U & (IData)(vlSelfRef.agnus__DOT__hpos)))) 
                           && (((IData)(vlSelfRef.agnus__DOT__vblend) 
                                | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprvstop) 
                                   & (~ (IData)(vlSelfRef.agnus__DOT__vbl)))) 
                               || (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprdmastate)));
    __Vtableidx1 = (((IData)(vlSelfRef.floppy_speed) 
                     << 9U) | (IData)(vlSelfRef.agnus__DOT__hpos));
    agnus__DOT__dsk1__DOT__dmaslot = Vagnus__ConstPool__TABLE_h3d6e5953_0
        [__Vtableidx1];
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18 
        = ((~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
               >> 4U)) & ((~ (IData)(agnus__DOT__bpd1__DOT__shres)) 
                          & (3U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7 
        = ((IData)(agnus__DOT__bpd1__DOT__shres) & 
           (0U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11 
        = ((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
             >> 4U) & (0U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))) 
           | ((IData)(agnus__DOT__bpd1__DOT__shres) 
              & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12)));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5 
        = (1U & ((~ (IData)(agnus__DOT__bpd1__DOT__shres)) 
                 & (~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
                       >> 4U))));
    agnus__DOT__cp1__DOT__dma_req = ((4U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                      ? ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                   >> 1U))) 
                                         && ((1U & 
                                              (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))) 
                                             || (1U 
                                                 & (~ 
                                                    ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                     & (IData)(agnus__DOT__cp1__DOT__illegalreg))))))
                                      : ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                   >> 1U))) 
                                         && (1U & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)))));
    agnus__DOT__dma_cop = (((IData)(agnus__DOT__cp1__DOT__dma_req) 
                            & (IData)(agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0)) 
                           & (IData)((0x280U == (0x280U 
                                                 & (IData)(vlSelfRef.agnus__DOT__dmacon)))));
    vlSelfRef.agnus__DOT__aud1__DOT__audptout = vlSelfRef.agnus__DOT__aud1__DOT__audpt
        [vlSelfRef.agnus__DOT__aud1__DOT__channel];
    __Vtableidx3 = vlSelfRef.agnus__DOT__aud1__DOT__channel;
    agnus__DOT__reg_address_aud = Vagnus__ConstPool__TABLE_h2dee94ce_0
        [__Vtableidx3];
    agnus__DOT__dma_ref = ((9U == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                           | ((IData)(vlSelfRef.strhor_paula) 
                              | ((0x11U == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                                 | (0x15U == (IData)(vlSelfRef.agnus__DOT__hpos)))));
    agnus__DOT__dma_spr = ((IData)(agnus__DOT__req_spr) 
                           & (IData)((0x220U == (0x220U 
                                                 & (IData)(vlSelfRef.agnus__DOT__dmacon)))));
    vlSelfRef.agnus__DOT__dma_dsk = ((IData)(vlSelfRef.disk_dmal) 
                                     & ((IData)(agnus__DOT__dsk1__DOT__dmaslot) 
                                        & (IData)(vlSelfRef.cck)));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6 
        = (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
            & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12)) 
           | (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
               >> 4U) & (3U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))));
    vlSelfRef.refresh = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                         && (IData)(agnus__DOT__dma_ref));
    vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq_match = 
        (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18) 
          & (7U == (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq))) 
         | (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6) 
             & (7U == (0xfU & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))) 
            | ((~ (IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18)) 
               & ((~ (IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6)) 
                  & (7U == (7U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))))));
    agnus__DOT__bpd1__DOT__plane = ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                     ? (1U & (~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))
                                     : ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                         ? ((2U & (
                                                   (~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                                                   << 1U)) 
                                            | (1U & 
                                               (~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                                   >> 1U))))
                                         : ((((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
                                              & (0U 
                                                 == 
                                                 (3U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))) 
                                             | ((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
                                                  >> 4U) 
                                                 & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12)) 
                                                | ((IData)(agnus__DOT__bpd1__DOT__shres) 
                                                   & (3U 
                                                      == 
                                                      (3U 
                                                       & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))))))
                                             ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14)
                                             : ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6)
                                                 ? 
                                                ((8U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                                                 | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14))
                                                 : 
                                                ((0x18U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                                                 | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14))))));
    agnus__DOT__address_bpl = ((vlSelfRef.agnus__DOT__bpd1__DOT__bplpth
                                [(7U & (IData)(agnus__DOT__bpd1__DOT__plane))] 
                                << 0xfU) | vlSelfRef.agnus__DOT__bpd1__DOT__bplptl
                               [(7U & (IData)(agnus__DOT__bpd1__DOT__plane))]);
    __Vtableidx4 = agnus__DOT__bpd1__DOT__plane;
    agnus__DOT__reg_address_bpl = Vagnus__ConstPool__TABLE_h7cec6eda_0
        [__Vtableidx4];
    vlSelfRef.agnus__DOT__dma_bpl = ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfrun) 
                                     & (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__dmaena_delayed) 
                                         >> 1U) & ((IData)(vlSelfRef.cck) 
                                                   & ((IData)(agnus__DOT__bpd1__DOT__plane) 
                                                      < 
                                                      (0xfU 
                                                       & ((IData)(vlSelfRef.aga)
                                                           ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed)
                                                           : 
                                                          ((7U 
                                                            == 
                                                            (7U 
                                                             & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed)))
                                                            ? 4U
                                                            : 
                                                           (7U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed)))))))));
    vlSelfRef.agnus__DOT__bpd1__DOT__newpt = (0xfffffU 
                                              & (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                                   ? 
                                                  ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_9) 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                                      >> 1U))
                                                   : 
                                                  ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                    ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_9)
                                                    : (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfend)))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(agnus__DOT__bpd1__DOT__plane))
                                                   ? 
                                                  (agnus__DOT__address_bpl 
                                                   + 
                                                   (((0xf8000U 
                                                      & ((- (IData)(
                                                                    (1U 
                                                                     & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan) 
                                                                        >> 0xeU)))) 
                                                         << 0xfU)) 
                                                     | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan)) 
                                                    + vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17))
                                                   : 
                                                  (agnus__DOT__address_bpl 
                                                   + 
                                                   (((0xf8000U 
                                                      & ((- (IData)(
                                                                    (1U 
                                                                     & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan) 
                                                                        >> 0xeU)))) 
                                                         << 0xfU)) 
                                                     | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan)) 
                                                    + vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17)))
                                                  : 
                                                 (agnus__DOT__address_bpl 
                                                  + vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17)));
    vlSelfRef.agnus__DOT__ack_spr = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                                     && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                                         && ((1U & 
                                              (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                             && ((1U 
                                                  & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                                 && (IData)(agnus__DOT__dma_spr)))));
    agnus__DOT__ack_cop = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                           && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                               && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                   && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                       && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                           && (IData)(agnus__DOT__dma_cop))))));
    vlSelfRef.agnus__DOT__bl1__DOT__enable = ((IData)(vlSelfRef.cck) 
                                              & ((~ 
                                                  ((IData)(vlSelfRef.agnus__DOT__dma_dsk) 
                                                   | ((IData)(agnus__DOT__dma_ref) 
                                                      | ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal) 
                                                         | ((IData)(agnus__DOT__dma_spr) 
                                                            | ((IData)(vlSelfRef.agnus__DOT__dma_bpl) 
                                                               | ((IData)(agnus__DOT__dma_cop) 
                                                                  | (3U 
                                                                     == (IData)(vlSelfRef.agnus__DOT__bls_cnt))))))))) 
                                                 & (IData)(vlSelfRef.agnus__DOT__blten)));
    vlSelfRef.agnus__DOT__cp1__DOT__dma_ack = ((IData)(agnus__DOT__ack_cop) 
                                               & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__enable));
    vlSelfRef.agnus__DOT__cp1__DOT__copper_next = (
                                                   (4U 
                                                    & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                    ? 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                     ? 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                      ? 
                                                     ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                       ? 7U
                                                       : 6U)
                                                      : 
                                                     ((1U 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                                       ? 
                                                      ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                        ? 6U
                                                        : 4U)
                                                       : 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__beam_match_wait)
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                         ? 6U
                                                         : 4U)
                                                        : 6U)))
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                      ? 
                                                     (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                       & (IData)(agnus__DOT__cp1__DOT__illegalreg))
                                                       ? 5U
                                                       : 
                                                      (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                        & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag))
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                         ? 4U
                                                         : 5U)
                                                        : 
                                                       ((1U 
                                                         & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                                         ? 
                                                        ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                          ? 7U
                                                          : 5U)
                                                         : 
                                                        ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                          ? 4U
                                                          : 5U))))
                                                      : 
                                                     ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                       ? 5U
                                                       : 4U)))
                                                    : 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                     ? 4U
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                      ? 4U
                                                      : 
                                                     ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                       ? 4U
                                                       : 0U))));
    vlSelfRef.agnus__DOT__bl1__DOT__decbsh = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
                                              & (0x18U 
                                                 == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)));
    vlSelfRef.int3 = ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_9) 
                        & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt))) 
                       | (0x18U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                      & ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__height_cnt)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)));
    vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_dec = 
        ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__next_word));
    agnus__DOT__dma_blt = ((((0x10U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                              ? ((1U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                        >> 3U)) && 
                                 ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                            >> 2U))) 
                                  && ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                       ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                          && (1U & 
                                              ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 9U)))
                                       : ((1U & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                          && (1U & 
                                              (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                >> 9U) 
                                               & ((IData)(
                                                          (0x12U 
                                                           != 
                                                           (0x12U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                                                  | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_pixel))))))))
                              : ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                  ? ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                               >> 2U))) 
                                     && (1U & ((2U 
                                                & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                ? (
                                                   (1U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                                   || (1U 
                                                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                          >> 9U)))
                                                : (
                                                   (1U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                    ? 
                                                   ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                    >> 0xbU)
                                                    : 
                                                   ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__used) 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__pipeline_full))))))
                                  : ((1U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                            >> 2U)) 
                                     && ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                   >> 1U))) 
                                         && (1U & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))))))) 
                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)) 
                           & (IData)(vlSelfRef.agnus__DOT__blten));
    vlSelfRef.agnus__DOT__cp1__DOT__skip = ((1U & ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                   >> 2U)) 
                                            && ((2U 
                                                 & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                 ? 
                                                ((1U 
                                                  & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))) 
                                                 && ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                     && (IData)(vlSelfRef.agnus__DOT__cp1__DOT__beam_match_skip)))
                                                 : 
                                                ((1U 
                                                  & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                  ? 
                                                 ((1U 
                                                   & (~ 
                                                      ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                       & (IData)(agnus__DOT__cp1__DOT__illegalreg)))) 
                                                  && (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag)) 
                                                      && (1U 
                                                          & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)))))
                                                  : (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag))));
    vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_rld = 
        (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_dec) 
          & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt))) 
         | ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)));
    vlSelfRef.dbr = ((IData)(vlSelfRef.agnus__DOT__dma_dsk) 
                     || ((IData)(agnus__DOT__dma_ref) 
                         || ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal) 
                             || ((IData)(vlSelfRef.agnus__DOT__dma_bpl) 
                                 || ((IData)(agnus__DOT__dma_spr) 
                                     || ((IData)(agnus__DOT__dma_cop) 
                                         || ((IData)(agnus__DOT__dma_blt) 
                                             & (3U 
                                                != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))))))));
    vlSelfRef.cpu_custom = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                            && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                                && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                    && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                        && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                            && ((1U 
                                                 & (~ (IData)(agnus__DOT__dma_cop))) 
                                                && (1U 
                                                    & (~ 
                                                       ((IData)(agnus__DOT__dma_blt) 
                                                        & (3U 
                                                           != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))))))))));
    agnus__DOT__ack_blt = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                           && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                               && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                   && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                       && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                           && ((1U 
                                                & (~ (IData)(agnus__DOT__dma_cop))) 
                                               && ((IData)(agnus__DOT__dma_blt) 
                                                   & (3U 
                                                      != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))))))));
    if ((0x10U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__blt_next = 
            ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
              ? ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 0U : ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                               ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? 0x1aU : 0x1bU)
                               : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? 0x18U : 0x1aU))
                           : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                               ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? 0x1bU : 0x19U)
                               : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__height_cnt))
                                       ? 0U : 0x19U)
                                   : 0x18U)))) : 0U);
        vlSelfRef.agnus__DOT__bl1__DOT__enaptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                                            : 
                                                           ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel)) 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)))
                                                           : (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable))));
    } else if ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__blt_next = 
            ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
              ? ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 0U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? 0U : ((IData)(vlSelfRef.cck)
                                    ? 4U : 0xcU))) : 
             ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
               ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                   ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                       ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_11)
                       : 0xbU) : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__used)
                                       ? 8U : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_12)
                                                ? 0U
                                                : 9U))
                                   : 0xaU)) : ((1U 
                                                & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                                    ? 
                                                   ((0x400U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))
                                                     ? 0xbU
                                                     : (IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_11))
                                                    : 9U)
                                                : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                                    ? 
                                                   ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_12)
                                                     ? 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__used)
                                                      ? 0xcU
                                                      : 0U)
                                                     : 9U)
                                                    : 8U))));
        vlSelfRef.agnus__DOT__bl1__DOT__enaptr = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && (IData)(agnus__DOT__ack_blt));
    } else {
        vlSelfRef.agnus__DOT__bl1__DOT__blt_next = 
            ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
              ? ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 0U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? 0U : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                    ? 0U : 4U))) : 
             ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
               ? 0U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                        ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                            ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))
                                ? 0x19U : 9U) : 1U)
                        : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                            ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__start)
                                ? 1U : 0U) : 0U))));
        vlSelfRef.agnus__DOT__bl1__DOT__enaptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && (IData)(agnus__DOT__ack_blt))));
    }
    agnus__DOT__we_blt = ((IData)(agnus__DOT__ack_blt) 
                          & (3U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__chsel)));
    if (vlSelfRef.agnus__DOT__dma_dsk) {
        vlSelfRef.agnus__DOT__dsk1__DOT__address_outnew 
            = (0xfffffU & ((IData)(1U) + vlSelfRef.agnus__DOT__address_dsk));
        vlSelfRef.agnus__DOT__reg_address = (0xffU 
                                             & ((IData)(vlSelfRef.disk_dmas)
                                                 ? 0x13U
                                                 : 4U));
    } else {
        vlSelfRef.agnus__DOT__dsk1__DOT__address_outnew 
            = (0xfffffU & vlSelfRef.__VdfgRegularize_h3c5bdd75_0_2);
        vlSelfRef.agnus__DOT__reg_address = (0xffU 
                                             & ((IData)(agnus__DOT__dma_ref)
                                                 ? 0xffU
                                                 : 
                                                ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal)
                                                  ? (IData)(agnus__DOT__reg_address_aud)
                                                  : 
                                                 ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                   ? (IData)(agnus__DOT__reg_address_bpl)
                                                   : 
                                                  ((IData)(agnus__DOT__dma_spr)
                                                    ? (IData)(agnus__DOT__reg_address_spr)
                                                    : 
                                                   ((IData)(agnus__DOT__dma_cop)
                                                     ? 
                                                    (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__enable) 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__selins))
                                                      ? 0x46U
                                                      : 
                                                     (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__enable) 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__selreg))
                                                       ? 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2) 
                                                       >> 1U)
                                                       : 0xffU))
                                                     : 
                                                    (((IData)(agnus__DOT__dma_blt) 
                                                      & (3U 
                                                         != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))
                                                      ? (IData)(vlSelfRef.agnus__DOT__reg_address_blt)
                                                      : 
                                                     (((IData)(vlSelfRef.aen) 
                                                       & ((IData)(vlSelfRef.rd) 
                                                          | ((IData)(vlSelfRef.hwr) 
                                                             | (IData)(vlSelfRef.lwr))))
                                                       ? (IData)(vlSelfRef.address_in)
                                                       : 0xffU))))))));
    }
    vlSelfRef.reg_address_out = vlSelfRef.agnus__DOT__reg_address;
    vlSelfRef.agnus__DOT__aud1__DOT__audlcena = (1U 
                                                 & (IData)(
                                                           ((0x40U 
                                                             == 
                                                             (0xc0U 
                                                              & (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                                                            & ((~ (IData)(
                                                                          (0U 
                                                                           != 
                                                                           (6U 
                                                                            & (IData)(vlSelfRef.agnus__DOT__reg_address))))) 
                                                               & (((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                                   >> 5U) 
                                                                  ^ 
                                                                  ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                                   >> 4U))))));
    vlSelfRef.agnus__DOT__aud1__DOT__audlcsel = ((2U 
                                                  & ((~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                       >> 4U)) 
                                                     << 1U)) 
                                                 | (1U 
                                                    & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                       >> 3U)));
    vlSelfRef.agnus__DOT__bpd1__DOT__bplptr_sel = (7U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                       ? (IData)(agnus__DOT__bpd1__DOT__plane)
                                                       : 
                                                      ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                       >> 1U)));
    if (vlSelfRef.agnus__DOT__ack_spr) {
        vlSelfRef.agnus__DOT__spr1__DOT__ptsel = (7U 
                                                  & (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite));
        vlSelfRef.agnus__DOT__spr1__DOT__pcsel = (7U 
                                                  & (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite));
    } else {
        vlSelfRef.agnus__DOT__spr1__DOT__ptsel = (7U 
                                                  & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                     >> 1U));
        vlSelfRef.agnus__DOT__spr1__DOT__pcsel = (7U 
                                                  & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                     >> 2U));
    }
    vlSelfRef.data_out = ((((2U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                            | (0x15U == (IData)(vlSelfRef.agnus__DOT__reg_address)))
                            ? (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame) 
                                << 0xfU) | (((IData)(vlSelfRef.ecs) 
                                             << 0xdU) 
                                            | (((IData)(vlSelfRef.ntsc) 
                                                << 0xcU) 
                                               | ((0x300U 
                                                   & ((- (IData)((IData)(vlSelfRef.aga))) 
                                                      << 8U)) 
                                                  | (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_line) 
                                                      << 7U) 
                                                     | (7U 
                                                        & ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                                           >> 8U)))))))
                            : (((3U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                                | (0x16U == (IData)(vlSelfRef.agnus__DOT__reg_address)))
                                ? ((0xff00U & ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                               << 8U)) 
                                   | (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r))
                                : 0U)) | (((1U == (IData)(vlSelfRef.agnus__DOT__reg_address))
                                            ? (((IData)(vlSelfRef.agnus__DOT__blit_busy) 
                                                << 0xeU) 
                                               | (((IData)(vlSelfRef.agnus__DOT__blit_zero) 
                                                   << 0xdU) 
                                                  | (IData)(vlSelfRef.agnus__DOT__dmacon)))
                                            : 0U) | 
                                          ((IData)(agnus__DOT__we_blt)
                                            ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltdhold)
                                            : 0U)));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel 
        = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enaptr)
            ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__ptrsel)
            : ((2U & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                      >> 2U)) | (1U & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                       >> 1U))));
    agnus__DOT__address_blt = ((vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth
                                [vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel] 
                                << 0xfU) | vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl
                               [vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel]);
    if (vlSelfRef.agnus__DOT__dma_dsk) {
        vlSelfRef.dbwe = (1U & (~ (IData)(vlSelfRef.disk_dmas)));
        vlSelfRef.address_out = vlSelfRef.agnus__DOT__address_dsk;
    } else {
        vlSelfRef.dbwe = (1U & ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                                && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                    && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                        && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                            && ((1U 
                                                 & (~ (IData)(agnus__DOT__dma_cop))) 
                                                && (((IData)(agnus__DOT__dma_blt) 
                                                     & (3U 
                                                        != (IData)(vlSelfRef.agnus__DOT__bls_cnt))) 
                                                    && (IData)(agnus__DOT__we_blt))))))));
        vlSelfRef.address_out = ((IData)(agnus__DOT__dma_ref)
                                  ? 0U : ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal)
                                           ? vlSelfRef.agnus__DOT__aud1__DOT__audpt
                                          [vlSelfRef.agnus__DOT__aud1__DOT__channel]
                                           : ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                               ? agnus__DOT__address_bpl
                                               : ((IData)(agnus__DOT__dma_spr)
                                                   ? vlSelfRef.agnus__DOT__address_spr
                                                   : 
                                                  ((IData)(agnus__DOT__dma_cop)
                                                    ? vlSelfRef.agnus__DOT__address_cop
                                                    : 
                                                   (((IData)(agnus__DOT__dma_blt) 
                                                     & (3U 
                                                        != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))
                                                     ? agnus__DOT__address_blt
                                                     : 0U))))));
    }
    agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr 
        = (0xfffffU & (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__decptr)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__incptr))
                        ? ((IData)(1U) + agnus__DOT__address_blt)
                        : (agnus__DOT__address_blt 
                           - ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__incptr)) 
                              & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__decptr)))));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__newptr 
        = (0xfffffU & (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__submod)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__addmod))
                        ? (agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr 
                           + vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT____VdfgRegularize_h62043dcc_0_0)
                        : (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__addmod)) 
                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__submod))
                            ? (agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr 
                               - vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT____VdfgRegularize_h62043dcc_0_0)
                            : agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr)));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_in 
        = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enaptr)
            ? vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__newptr
            : vlSelfRef.__VdfgRegularize_h3c5bdd75_0_2);
}

void Vagnus___024root___eval_triggers__ico(Vagnus___024root* vlSelf);

bool Vagnus___024root___eval_phase__ico(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_phase__ico\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vagnus___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vagnus___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vagnus___024root___act_sequent__TOP__0(Vagnus___024root* vlSelf);

void Vagnus___024root___eval_act(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_act\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VactTriggered.word(0U))) {
        Vagnus___024root___act_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus___024root___act_sequent__TOP__0(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___act_sequent__TOP__0\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (2U & (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                     << 1U) ^ (0xfffffffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (4U & ((0xfffffffcU & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                    << 1U)) ^ (0xfffffffcU 
                                               & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (8U & ((0xfffffff8U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                    << 1U)) ^ (0xfffffff8U 
                                               & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x10U & ((0xfffffff0U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                       << 1U)) ^ (0xfffffff0U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x20U & ((0xffffffe0U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                       << 1U)) ^ (0xffffffe0U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x40U & ((0xffffffc0U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                       << 1U)) ^ (0xffffffc0U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x80U & ((0xffffff80U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                       << 1U)) ^ (0xffffff80U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x100U & ((0xffffff00U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                        << 1U)) ^ (0xffffff00U 
                                                   & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x200U & ((0xfffffe00U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                        << 1U)) ^ (0xfffffe00U 
                                                   & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x400U & ((0xfffffc00U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                        << 1U)) ^ (0xfffffc00U 
                                                   & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x800U & ((0xfffff800U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                        << 1U)) ^ (0xfffff800U 
                                                   & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x1000U & ((0xfffff000U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                         << 1U)) ^ 
                         (0xfffff000U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x2000U & ((0xffffe000U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                         << 1U)) ^ 
                         (0xffffe000U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x4000U & ((0xffffc000U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                         << 1U)) ^ 
                         (0xffffc000U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (0x8000U & ((0xffff8000U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                         << 1U)) ^ 
                         (0xffff8000U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)))));
    vlSelfRef.agnus__DOT__bl1__DOT__fill_out = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__efe)
                                                 ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)
                                                 : 
                                                ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__ife)
                                                  ? 
                                                 ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                                  | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out))
                                                  : (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out)));
}

void Vagnus___024root___nba_sequent__TOP__0(Vagnus___024root* vlSelf);

void Vagnus___024root___eval_nba(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_nba\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((2ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus___024root___nba_sequent__TOP__0(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus___024root___act_sequent__TOP__0(vlSelf);
    }
}

extern const VlUnpacked<IData/*17:0*/, 32> Vagnus__ConstPool__TABLE_hbbf19d2e_0;
extern const VlUnpacked<CData/*7:0*/, 4> Vagnus__ConstPool__TABLE_h78219485_0;

VL_INLINE_OPT void Vagnus___024root___nba_sequent__TOP__0(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___nba_sequent__TOP__0\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus__DOT__req_spr;
    agnus__DOT__req_spr = 0;
    CData/*0:0*/ agnus__DOT__dma_spr;
    agnus__DOT__dma_spr = 0;
    CData/*0:0*/ agnus__DOT__dma_cop;
    agnus__DOT__dma_cop = 0;
    CData/*0:0*/ agnus__DOT__dma_blt;
    agnus__DOT__dma_blt = 0;
    CData/*0:0*/ agnus__DOT__dma_ref;
    agnus__DOT__dma_ref = 0;
    CData/*0:0*/ agnus__DOT__ack_cop;
    agnus__DOT__ack_cop = 0;
    CData/*0:0*/ agnus__DOT__ack_blt;
    agnus__DOT__ack_blt = 0;
    CData/*7:0*/ agnus__DOT__reg_address_aud;
    agnus__DOT__reg_address_aud = 0;
    IData/*19:0*/ agnus__DOT__address_bpl;
    agnus__DOT__address_bpl = 0;
    CData/*7:0*/ agnus__DOT__reg_address_bpl;
    agnus__DOT__reg_address_bpl = 0;
    CData/*7:0*/ agnus__DOT__reg_address_spr;
    agnus__DOT__reg_address_spr = 0;
    IData/*19:0*/ agnus__DOT__address_blt;
    agnus__DOT__address_blt = 0;
    CData/*0:0*/ agnus__DOT__we_blt;
    agnus__DOT__we_blt = 0;
    CData/*0:0*/ agnus__DOT__dsk1__DOT__dmaslot;
    agnus__DOT__dsk1__DOT__dmaslot = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT__shres;
    agnus__DOT__bpd1__DOT__shres = 0;
    CData/*4:0*/ agnus__DOT__bpd1__DOT__plane;
    agnus__DOT__bpd1__DOT__plane = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11 = 0;
    SData/*14:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18 = 0;
    CData/*0:0*/ agnus__DOT__cp1__DOT__illegalreg;
    agnus__DOT__cp1__DOT__illegalreg = 0;
    CData/*0:0*/ agnus__DOT__cp1__DOT__dma_req;
    agnus__DOT__cp1__DOT__dma_req = 0;
    CData/*0:0*/ agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0;
    agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0 = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT__lineinc;
    agnus__DOT__bl1__DOT__lineinc = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT__linedec;
    agnus__DOT__bl1__DOT__linedec = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT__lineadd;
    agnus__DOT__bl1__DOT__lineadd = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT__linesub;
    agnus__DOT__bl1__DOT__linesub = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0;
    agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0 = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0;
    agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0 = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13;
    agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13 = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14;
    agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14 = 0;
    IData/*17:0*/ agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot;
    agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot = 0;
    QData/*35:0*/ agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new;
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new = 0;
    QData/*35:0*/ agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old;
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old = 0;
    IData/*17:0*/ agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot;
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot = 0;
    IData/*19:0*/ agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr;
    agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr = 0;
    SData/*10:0*/ agnus__DOT__bc1__DOT__vbstop;
    agnus__DOT__bc1__DOT__vbstop = 0;
    CData/*0:0*/ agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_0;
    agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_0 = 0;
    CData/*0:0*/ agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_1;
    agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_1 = 0;
    SData/*9:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    SData/*8:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    CData/*1:0*/ __Vtableidx3;
    __Vtableidx3 = 0;
    CData/*4:0*/ __Vtableidx4;
    __Vtableidx4 = 0;
    CData/*1:0*/ __Vtableidx5;
    __Vtableidx5 = 0;
    CData/*4:0*/ __Vtableidx6;
    __Vtableidx6 = 0;
    CData/*4:0*/ __Vtableidx7;
    __Vtableidx7 = 0;
    SData/*12:0*/ __Vdly__agnus__DOT__dmacon;
    __Vdly__agnus__DOT__dmacon = 0;
    CData/*1:0*/ __Vdly__agnus__DOT__bls_cnt;
    __Vdly__agnus__DOT__bls_cnt = 0;
    SData/*8:0*/ __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt;
    __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt = 0;
    SData/*8:0*/ __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop;
    __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop = 0;
    SData/*10:0*/ __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde;
    __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde = 0;
    CData/*1:0*/ __Vdly__agnus__DOT__bpd1__DOT__dmaena_delayed;
    __Vdly__agnus__DOT__bpd1__DOT__dmaena_delayed = 0;
    CData/*0:0*/ __Vdly__agnus__DOT__bpd1__DOT__ddfend;
    __Vdly__agnus__DOT__bpd1__DOT__ddfend = 0;
    CData/*2:0*/ __Vdly__agnus__DOT__spr1__DOT__sprsel;
    __Vdly__agnus__DOT__spr1__DOT__sprsel = 0;
    IData/*19:0*/ __Vdly__agnus__DOT__address_cop;
    __Vdly__agnus__DOT__address_cop = 0;
    SData/*15:0*/ __Vdly__agnus__DOT__bl1__DOT__bltcon0;
    __Vdly__agnus__DOT__bl1__DOT__bltcon0 = 0;
    SData/*15:0*/ __Vdly__agnus__DOT__bl1__DOT__bltcon1;
    __Vdly__agnus__DOT__bl1__DOT__bltcon1 = 0;
    SData/*10:0*/ __Vdly__agnus__DOT__bl1__DOT__width_cnt;
    __Vdly__agnus__DOT__bl1__DOT__width_cnt = 0;
    SData/*14:0*/ __Vdly__agnus__DOT__bl1__DOT__height_cnt;
    __Vdly__agnus__DOT__bl1__DOT__height_cnt = 0;
    CData/*7:0*/ __Vdly__agnus__DOT__bc1__DOT__hpos_r;
    __Vdly__agnus__DOT__bc1__DOT__hpos_r = 0;
    CData/*0:0*/ __Vdly__agnus__DOT__bc1__DOT__long_line;
    __Vdly__agnus__DOT__bc1__DOT__long_line = 0;
    SData/*10:0*/ __Vdly__agnus__DOT__vpos;
    __Vdly__agnus__DOT__vpos = 0;
    CData/*0:0*/ __Vdly__agnus__DOT__bc1__DOT__long_frame;
    __Vdly__agnus__DOT__bc1__DOT__long_frame = 0;
    CData/*4:0*/ __VdlyVal__agnus__DOT__aud1__DOT__audlch__v0;
    __VdlyVal__agnus__DOT__aud1__DOT__audlch__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus__DOT__aud1__DOT__audlch__v0;
    __VdlyDim0__agnus__DOT__aud1__DOT__audlch__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__aud1__DOT__audlch__v0;
    __VdlySet__agnus__DOT__aud1__DOT__audlch__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus__DOT__aud1__DOT__audlcl__v0;
    __VdlyVal__agnus__DOT__aud1__DOT__audlcl__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus__DOT__aud1__DOT__audlcl__v0;
    __VdlyDim0__agnus__DOT__aud1__DOT__audlcl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__aud1__DOT__audlcl__v0;
    __VdlySet__agnus__DOT__aud1__DOT__audlcl__v0 = 0;
    IData/*19:0*/ __VdlyVal__agnus__DOT__aud1__DOT__audpt__v0;
    __VdlyVal__agnus__DOT__aud1__DOT__audpt__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus__DOT__aud1__DOT__audpt__v0;
    __VdlyDim0__agnus__DOT__aud1__DOT__audpt__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__aud1__DOT__audpt__v0;
    __VdlySet__agnus__DOT__aud1__DOT__audpt__v0 = 0;
    CData/*4:0*/ __VdlyVal__agnus__DOT__bpd1__DOT__bplpth__v0;
    __VdlyVal__agnus__DOT__bpd1__DOT__bplpth__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus__DOT__bpd1__DOT__bplpth__v0;
    __VdlyDim0__agnus__DOT__bpd1__DOT__bplpth__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__bpd1__DOT__bplpth__v0;
    __VdlySet__agnus__DOT__bpd1__DOT__bplpth__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus__DOT__bpd1__DOT__bplptl__v0;
    __VdlyVal__agnus__DOT__bpd1__DOT__bplptl__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus__DOT__bpd1__DOT__bplptl__v0;
    __VdlyDim0__agnus__DOT__bpd1__DOT__bplptl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__bpd1__DOT__bplptl__v0;
    __VdlySet__agnus__DOT__bpd1__DOT__bplptl__v0 = 0;
    CData/*5:0*/ __VdlyVal__agnus__DOT__bpd1__DOT__bplcon0_delay__v0;
    __VdlyVal__agnus__DOT__bpd1__DOT__bplcon0_delay__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__bpd1__DOT__bplcon0_delay__v0;
    __VdlySet__agnus__DOT__bpd1__DOT__bplcon0_delay__v0 = 0;
    CData/*5:0*/ __VdlyVal__agnus__DOT__bpd1__DOT__bplcon0_delay__v1;
    __VdlyVal__agnus__DOT__bpd1__DOT__bplcon0_delay__v1 = 0;
    CData/*4:0*/ __VdlyVal__agnus__DOT__spr1__DOT__sprpth__v0;
    __VdlyVal__agnus__DOT__spr1__DOT__sprpth__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus__DOT__spr1__DOT__sprpth__v0;
    __VdlyDim0__agnus__DOT__spr1__DOT__sprpth__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__spr1__DOT__sprpth__v0;
    __VdlySet__agnus__DOT__spr1__DOT__sprpth__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus__DOT__spr1__DOT__sprptl__v0;
    __VdlyVal__agnus__DOT__spr1__DOT__sprptl__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus__DOT__spr1__DOT__sprptl__v0;
    __VdlyDim0__agnus__DOT__spr1__DOT__sprptl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__spr1__DOT__sprptl__v0;
    __VdlySet__agnus__DOT__spr1__DOT__sprptl__v0 = 0;
    CData/*7:0*/ __VdlyVal__agnus__DOT__spr1__DOT__sprpos__v0;
    __VdlyVal__agnus__DOT__spr1__DOT__sprpos__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus__DOT__spr1__DOT__sprpos__v0;
    __VdlyDim0__agnus__DOT__spr1__DOT__sprpos__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__spr1__DOT__sprpos__v0;
    __VdlySet__agnus__DOT__spr1__DOT__sprpos__v0 = 0;
    CData/*0:0*/ __VdlyVal__agnus__DOT__spr1__DOT__sprposh__v0;
    __VdlyVal__agnus__DOT__spr1__DOT__sprposh__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus__DOT__spr1__DOT__sprposh__v0;
    __VdlyDim0__agnus__DOT__spr1__DOT__sprposh__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__spr1__DOT__sprposh__v0;
    __VdlySet__agnus__DOT__spr1__DOT__sprposh__v0 = 0;
    SData/*11:0*/ __VdlyVal__agnus__DOT__spr1__DOT__sprctl__v0;
    __VdlyVal__agnus__DOT__spr1__DOT__sprctl__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus__DOT__spr1__DOT__sprctl__v0;
    __VdlyDim0__agnus__DOT__spr1__DOT__sprctl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__spr1__DOT__sprctl__v0;
    __VdlySet__agnus__DOT__spr1__DOT__sprctl__v0 = 0;
    CData/*0:0*/ __VdlyVal__agnus__DOT__spr1__DOT__dmastate_mem__v0;
    __VdlyVal__agnus__DOT__spr1__DOT__dmastate_mem__v0 = 0;
    CData/*2:0*/ __VdlyDim0__agnus__DOT__spr1__DOT__dmastate_mem__v0;
    __VdlyDim0__agnus__DOT__spr1__DOT__dmastate_mem__v0 = 0;
    CData/*4:0*/ __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0;
    __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0;
    __VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0;
    __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0;
    __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0;
    __VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0;
    __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0 = 0;
    SData/*14:0*/ __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0;
    __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0 = 0;
    CData/*1:0*/ __VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0;
    __VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0 = 0;
    CData/*0:0*/ __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0;
    __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0 = 0;
    // Body
    __VdlySet__agnus__DOT__spr1__DOT__sprposh__v0 = 0U;
    __VdlySet__agnus__DOT__aud1__DOT__audlch__v0 = 0U;
    __VdlySet__agnus__DOT__aud1__DOT__audlcl__v0 = 0U;
    __VdlySet__agnus__DOT__spr1__DOT__sprpos__v0 = 0U;
    __VdlySet__agnus__DOT__spr1__DOT__sprpth__v0 = 0U;
    __VdlySet__agnus__DOT__spr1__DOT__sprptl__v0 = 0U;
    __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0 = 0U;
    __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0 = 0U;
    __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0 = 0U;
    __VdlySet__agnus__DOT__spr1__DOT__sprctl__v0 = 0U;
    __VdlySet__agnus__DOT__aud1__DOT__audpt__v0 = 0U;
    __Vdly__agnus__DOT__bl1__DOT__width_cnt = vlSelfRef.agnus__DOT__bl1__DOT__width_cnt;
    __VdlySet__agnus__DOT__bpd1__DOT__bplpth__v0 = 0U;
    __VdlySet__agnus__DOT__bpd1__DOT__bplptl__v0 = 0U;
    __Vdly__agnus__DOT__bc1__DOT__long_frame = vlSelfRef.agnus__DOT__bc1__DOT__long_frame;
    __Vdly__agnus__DOT__bl1__DOT__height_cnt = vlSelfRef.agnus__DOT__bl1__DOT__height_cnt;
    __Vdly__agnus__DOT__spr1__DOT__sprsel = vlSelfRef.agnus__DOT__spr1__DOT__sprsel;
    __Vdly__agnus__DOT__bls_cnt = vlSelfRef.agnus__DOT__bls_cnt;
    __Vdly__agnus__DOT__bl1__DOT__bltcon1 = vlSelfRef.agnus__DOT__bl1__DOT__bltcon1;
    __Vdly__agnus__DOT__bpd1__DOT__dmaena_delayed = vlSelfRef.agnus__DOT__bpd1__DOT__dmaena_delayed;
    __Vdly__agnus__DOT__dmacon = vlSelfRef.agnus__DOT__dmacon;
    __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt 
        = vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt;
    __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop 
        = vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop;
    __Vdly__agnus__DOT__bl1__DOT__bltcon0 = vlSelfRef.agnus__DOT__bl1__DOT__bltcon0;
    __Vdly__agnus__DOT__vpos = vlSelfRef.agnus__DOT__vpos;
    __VdlySet__agnus__DOT__bpd1__DOT__bplcon0_delay__v0 = 0U;
    __Vdly__agnus__DOT__bc1__DOT__hpos_r = vlSelfRef.agnus__DOT__bc1__DOT__hpos_r;
    __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde 
        = vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde;
    __Vdly__agnus__DOT__address_cop = vlSelfRef.agnus__DOT__address_cop;
    __Vdly__agnus__DOT__bc1__DOT__long_line = vlSelfRef.agnus__DOT__bc1__DOT__long_line;
    __Vdly__agnus__DOT__bpd1__DOT__ddfend = vlSelfRef.agnus__DOT__bpd1__DOT__ddfend;
    __VdlyVal__agnus__DOT__spr1__DOT__dmastate_mem__v0 
        = vlSelfRef.agnus__DOT__spr1__DOT__dmastate_in;
    __VdlyDim0__agnus__DOT__spr1__DOT__dmastate_mem__v0 
        = vlSelfRef.agnus__DOT__spr1__DOT__sprsel;
    if (((1U & ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprsel) 
                >> 2U)) == (1U & (IData)(vlSelfRef.agnus__DOT__hpos)))) {
        __Vdly__agnus__DOT__spr1__DOT__sprsel = (7U 
                                                 & ((IData)(1U) 
                                                    + (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprsel)));
    }
    if ((IData)(((2U == (6U & (IData)(vlSelfRef.agnus__DOT__hpos))) 
                 & ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                    == (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprsel))))) {
        vlSelfRef.agnus__DOT__spr1__DOT__sprvstop = 
            (((0xfffffe00U & (((IData)(vlSelfRef.ecs) 
                               << 9U) & (IData)(vlSelfRef.agnus__DOT__spr1__DOT__vstop))) 
              | (0x1ffU & (IData)(vlSelfRef.agnus__DOT__spr1__DOT__vstop))) 
             == (0x3ffU & (IData)(vlSelfRef.agnus__DOT__vpos)));
        vlSelfRef.agnus__DOT__spr1__DOT__sprdmastate 
            = (vlSelfRef.agnus__DOT__spr1__DOT__dmastate_mem
               [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
               & (~ ((((IData)(vlSelfRef.agnus__DOT__spr1__DOT__fmode) 
                       >> 0xfU) & vlSelfRef.agnus__DOT__spr1__DOT__sprposh
                      [vlSelfRef.agnus__DOT__spr1__DOT__sprsel]) 
                     & ((1U & (IData)(vlSelfRef.agnus__DOT__vpos)) 
                        != (1U & (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_3))))));
    }
    if (vlSelfRef.clk7_en) {
        if ((IData)((0xa0U == (0xe3U & (IData)(vlSelfRef.agnus__DOT__reg_address))))) {
            __VdlyVal__agnus__DOT__spr1__DOT__sprposh__v0 
                = (1U & ((IData)(vlSelfRef.data_in) 
                         >> 7U));
            __VdlyDim0__agnus__DOT__spr1__DOT__sprposh__v0 
                = vlSelfRef.agnus__DOT__spr1__DOT__pcsel;
            __VdlySet__agnus__DOT__spr1__DOT__sprposh__v0 = 1U;
            __VdlyVal__agnus__DOT__spr1__DOT__sprpos__v0 
                = (0xffU & ((IData)(vlSelfRef.data_in) 
                            >> 8U));
            __VdlyDim0__agnus__DOT__spr1__DOT__sprpos__v0 
                = vlSelfRef.agnus__DOT__spr1__DOT__pcsel;
            __VdlySet__agnus__DOT__spr1__DOT__sprpos__v0 = 1U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__aud1__DOT__audlcena) 
             & (~ (IData)(vlSelfRef.agnus__DOT__reg_address)))) {
            __VdlyVal__agnus__DOT__aud1__DOT__audlch__v0 
                = (0x1fU & (IData)(vlSelfRef.data_in));
            __VdlyDim0__agnus__DOT__aud1__DOT__audlch__v0 
                = vlSelfRef.agnus__DOT__aud1__DOT__audlcsel;
            __VdlySet__agnus__DOT__aud1__DOT__audlch__v0 = 1U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__aud1__DOT__audlcena) 
             & (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            __VdlyVal__agnus__DOT__aud1__DOT__audlcl__v0 
                = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                              >> 1U));
            __VdlyDim0__agnus__DOT__aud1__DOT__audlcl__v0 
                = vlSelfRef.agnus__DOT__aud1__DOT__audlcsel;
            __VdlySet__agnus__DOT__aud1__DOT__audlcl__v0 = 1U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__ack_spr) 
             | (IData)((0x90U == (0xf1U & (IData)(vlSelfRef.agnus__DOT__reg_address)))))) {
            __VdlyVal__agnus__DOT__spr1__DOT__sprpth__v0 
                = (0x1fU & ((IData)(vlSelfRef.agnus__DOT__ack_spr)
                             ? (vlSelfRef.agnus__DOT__spr1__DOT__newptr 
                                >> 0xfU) : (IData)(vlSelfRef.data_in)));
            __VdlyDim0__agnus__DOT__spr1__DOT__sprpth__v0 
                = vlSelfRef.agnus__DOT__spr1__DOT__ptsel;
            __VdlySet__agnus__DOT__spr1__DOT__sprpth__v0 = 1U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__ack_spr) 
             | (IData)((0x91U == (0xf1U & (IData)(vlSelfRef.agnus__DOT__reg_address)))))) {
            __VdlyVal__agnus__DOT__spr1__DOT__sprptl__v0 
                = (0x7fffU & ((IData)(vlSelfRef.agnus__DOT__ack_spr)
                               ? vlSelfRef.agnus__DOT__spr1__DOT__newptr
                               : ((IData)(vlSelfRef.data_in) 
                                  >> 1U)));
            __VdlyDim0__agnus__DOT__spr1__DOT__sprptl__v0 
                = vlSelfRef.agnus__DOT__spr1__DOT__ptsel;
            __VdlySet__agnus__DOT__spr1__DOT__sprptl__v0 = 1U;
        }
        if ((0xcU == (0x3fU & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                               >> 2U)))) {
            __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0 
                = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                              >> 1U));
            __VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0 
                = (3U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enaptr)
                          ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__modsel)
                          : (IData)(vlSelfRef.agnus__DOT__reg_address)));
            __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0 = 1U;
        }
        if ((((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enaptr) 
                | (0x28U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
               | (0x26U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
              | (0x24U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
             | (0x2aU == (IData)(vlSelfRef.agnus__DOT__reg_address)))) {
            __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0 
                = (0x1fU & (vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_in 
                            >> 0xfU));
            __VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0 
                = vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel;
            __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0 = 1U;
        }
        if ((((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enaptr) 
                | (0x29U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
               | (0x27U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
              | (0x25U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
             | (0x2bU == (IData)(vlSelfRef.agnus__DOT__reg_address)))) {
            __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0 
                = (0x7fffU & vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_in);
            __VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0 
                = vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel;
            __VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0 = 1U;
        }
        if ((IData)((0xa1U == (0xe3U & (IData)(vlSelfRef.agnus__DOT__reg_address))))) {
            __VdlyVal__agnus__DOT__spr1__DOT__sprctl__v0 
                = ((0xff0U & ((IData)(vlSelfRef.data_in) 
                              >> 4U)) | ((0xcU & ((IData)(vlSelfRef.data_in) 
                                                  >> 3U)) 
                                         | (3U & ((IData)(vlSelfRef.data_in) 
                                                  >> 1U))));
            __VdlyDim0__agnus__DOT__spr1__DOT__sprctl__v0 
                = vlSelfRef.agnus__DOT__spr1__DOT__pcsel;
            __VdlySet__agnus__DOT__spr1__DOT__sprctl__v0 = 1U;
        }
        if (vlSelfRef.agnus__DOT__aud1__DOT__dmal) {
            __VdlyVal__agnus__DOT__aud1__DOT__audpt__v0 
                = (0xfffffU & ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmas)
                                ? ((vlSelfRef.agnus__DOT__aud1__DOT__audlch
                                    [vlSelfRef.agnus__DOT__aud1__DOT__channel] 
                                    << 0xfU) | vlSelfRef.agnus__DOT__aud1__DOT__audlcl
                                   [vlSelfRef.agnus__DOT__aud1__DOT__channel])
                                : ((IData)(1U) + vlSelfRef.agnus__DOT__aud1__DOT__audptout)));
            __VdlyDim0__agnus__DOT__aud1__DOT__audpt__v0 
                = vlSelfRef.agnus__DOT__aud1__DOT__channel;
            __VdlySet__agnus__DOT__aud1__DOT__audpt__v0 = 1U;
        }
        if (vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_rld) {
            __Vdly__agnus__DOT__bl1__DOT__width_cnt 
                = vlSelfRef.agnus__DOT__bl1__DOT__width;
        } else if (vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_dec) {
            __Vdly__agnus__DOT__bl1__DOT__width_cnt 
                = (0x7ffU & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt) 
                             - (IData)(1U)));
        }
        if (((IData)(vlSelfRef.agnus__DOT__dma_bpl) 
             | (IData)((0x70U == (0xf1U & (IData)(vlSelfRef.agnus__DOT__reg_address)))))) {
            __VdlyVal__agnus__DOT__bpd1__DOT__bplpth__v0 
                = (0x1fU & ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                             ? (vlSelfRef.agnus__DOT__bpd1__DOT__newpt 
                                >> 0xfU) : (IData)(vlSelfRef.data_in)));
            __VdlyDim0__agnus__DOT__bpd1__DOT__bplpth__v0 
                = vlSelfRef.agnus__DOT__bpd1__DOT__bplptr_sel;
            __VdlySet__agnus__DOT__bpd1__DOT__bplpth__v0 = 1U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__dma_bpl) 
             | (IData)((0x71U == (0xf1U & (IData)(vlSelfRef.agnus__DOT__reg_address)))))) {
            __VdlyVal__agnus__DOT__bpd1__DOT__bplptl__v0 
                = (0x7fffU & ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                               ? vlSelfRef.agnus__DOT__bpd1__DOT__newpt
                               : ((IData)(vlSelfRef.data_in) 
                                  >> 1U)));
            __VdlyDim0__agnus__DOT__bpd1__DOT__bplptl__v0 
                = vlSelfRef.agnus__DOT__bpd1__DOT__bplptr_sel;
            __VdlySet__agnus__DOT__bpd1__DOT__bplptl__v0 = 1U;
        }
        if ((0x2cU == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            __Vdly__agnus__DOT__bl1__DOT__height_cnt 
                = ((0x400U & ((~ (0U != (0x3ffU & ((IData)(vlSelfRef.data_in) 
                                                   >> 6U)))) 
                              << 0xaU)) | (0x3ffU & 
                                           ((IData)(vlSelfRef.data_in) 
                                            >> 6U)));
            vlSelfRef.agnus__DOT__bl1__DOT__width = 
                ((0x40U & ((~ (0U != (0x3fU & (IData)(vlSelfRef.data_in)))) 
                           << 6U)) | (0x3fU & (IData)(vlSelfRef.data_in)));
        } else if (((0x2fU == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                    & (IData)(vlSelfRef.ecs))) {
            __Vdly__agnus__DOT__bl1__DOT__height_cnt 
                = vlSelfRef.agnus__DOT__bl1__DOT__height;
            vlSelfRef.agnus__DOT__bl1__DOT__width = 
                (0x7ffU & (IData)(vlSelfRef.data_in));
        } else if ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__next_word)) 
                    & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt)))) {
            __Vdly__agnus__DOT__bl1__DOT__height_cnt 
                = (0x7fffU & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__height_cnt) 
                              - (IData)(1U)));
        }
        if ((1U & (~ (IData)(vlSelfRef.cck)))) {
            if ((1U & ((~ (IData)(vlSelfRef.bls)) | 
                       ((IData)(vlSelfRef.agnus__DOT__dmacon) 
                        >> 0xaU)))) {
                __Vdly__agnus__DOT__bls_cnt = 0U;
            } else if ((3U != (IData)(vlSelfRef.agnus__DOT__bls_cnt))) {
                __Vdly__agnus__DOT__bls_cnt = (3U & 
                                               ((IData)(1U) 
                                                + (IData)(vlSelfRef.agnus__DOT__bls_cnt)));
            }
        }
        if ((3U == (3U & (IData)(vlSelfRef.agnus__DOT__hpos)))) {
            __Vdly__agnus__DOT__bpd1__DOT__dmaena_delayed 
                = ((2U & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__dmaena_delayed) 
                          << 1U)) | (IData)(vlSelfRef.agnus__DOT__bplen));
        }
        if ((0x15U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            __Vdly__agnus__DOT__vpos = ((0xffU & (IData)(__Vdly__agnus__DOT__vpos)) 
                                        | (0x700U & 
                                           ((IData)(vlSelfRef.data_in) 
                                            << 8U)));
        } else if ((0x16U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            __Vdly__agnus__DOT__vpos = ((0x700U & (IData)(__Vdly__agnus__DOT__vpos)) 
                                        | (0xffU & 
                                           ((IData)(vlSelfRef.data_in) 
                                            >> 8U)));
        } else if (vlSelfRef.agnus__DOT__bc1__DOT__vpos_inc) {
            __Vdly__agnus__DOT__vpos = ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__last_line)
                                         ? 0U : (0x7ffU 
                                                 & ((IData)(1U) 
                                                    + (IData)(vlSelfRef.agnus__DOT__vpos))));
        }
        if ((0x16U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            __Vdly__agnus__DOT__bc1__DOT__hpos_r = 
                (0xffU & (IData)(vlSelfRef.data_in));
        } else if (vlSelfRef.agnus__DOT__bc1__DOT__end_of_line) {
            __Vdly__agnus__DOT__bc1__DOT__hpos_r = 0U;
        } else if (((IData)(vlSelfRef.cck) & ((~ (IData)(vlSelfRef.agnus__DOT__bc1__DOT__ersy)) 
                                              | (0U 
                                                 != (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r))))) {
            __Vdly__agnus__DOT__bc1__DOT__hpos_r = 
                (0xffU & ((IData)(1U) + (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r)));
        }
        if ((((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack) 
              & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__strobe1)) 
             & (0U == (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)))) {
            __Vdly__agnus__DOT__address_cop = (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__cop1lch) 
                                                << 0xfU) 
                                               | (IData)(vlSelfRef.agnus__DOT__cp1__DOT__cop1lcl));
        } else if ((((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack) 
                     & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__strobe2)) 
                    & (0U == (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)))) {
            __Vdly__agnus__DOT__address_cop = (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__cop2lch) 
                                                << 0xfU) 
                                               | (IData)(vlSelfRef.agnus__DOT__cp1__DOT__cop2lcl));
        } else if (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack) 
                    & ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__selins) 
                       | (IData)(vlSelfRef.agnus__DOT__cp1__DOT__selreg)))) {
            __Vdly__agnus__DOT__address_cop = (0xfffffU 
                                               & ((IData)(1U) 
                                                  + vlSelfRef.agnus__DOT__address_cop));
        }
        if (vlSelfRef.agnus__DOT__bc1__DOT__end_of_line) {
            if (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__pal) 
                 | (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
                     >> 0xbU) & (IData)(vlSelfRef.varbeamen)))) {
                __Vdly__agnus__DOT__bc1__DOT__long_line = 0U;
            } else if ((1U & (~ (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
                                  >> 0xbU) & (IData)(vlSelfRef.varbeamen))))) {
                __Vdly__agnus__DOT__bc1__DOT__long_line 
                    = (1U & (~ (IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_line)));
            }
        }
        if ((((IData)(vlSelfRef.agnus__DOT__hpos) & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq_match)) 
             & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfend))) {
            __Vdly__agnus__DOT__bpd1__DOT__ddfend = 0U;
        } else if ((((IData)(vlSelfRef.agnus__DOT__hpos) 
                     & (7U == (7U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))) 
                    & (~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfena)))) {
            __Vdly__agnus__DOT__bpd1__DOT__ddfend = 1U;
        }
        if ((1U & (IData)(vlSelfRef.agnus__DOT__hpos))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq 
                = ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfrun)
                    ? (0x1fU & ((IData)(1U) + (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))
                    : 0U);
            __VdlyVal__agnus__DOT__bpd1__DOT__bplcon0_delay__v0 
                = vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0;
            __VdlySet__agnus__DOT__bpd1__DOT__bplcon0_delay__v0 = 1U;
            __VdlyVal__agnus__DOT__bpd1__DOT__bplcon0_delay__v1 
                = vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delay
                [0U];
            vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed 
                = vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delay
                [1U];
            if (((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfena) 
                   & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__vdiwena)) 
                  & (~ ((IData)(vlSelfRef.agnus__DOT__hpos) 
                        >> 1U))) & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__dmaena_delayed))) {
                vlSelfRef.agnus__DOT__bpd1__DOT__ddfrun = 1U;
            } else if ((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfend) 
                         | (~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__vdiwena))) 
                        & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq_match))) {
                vlSelfRef.agnus__DOT__bpd1__DOT__ddfrun = 0U;
            }
            vlSelfRef.agnus__DOT__bpd1__DOT__ddfena 
                = vlSelfRef.agnus__DOT__bpd1__DOT__ddfena_0;
            vlSelfRef.agnus__DOT__bpd1__DOT__ddfena_0 
                = (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__hardena) 
                    | (IData)(vlSelfRef.harddis)) & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__softena));
            if (vlSelfRef.agnus__DOT__bpd1__DOT__hard_start) {
                vlSelfRef.agnus__DOT__bpd1__DOT__hardena = 1U;
            } else if (vlSelfRef.agnus__DOT__bpd1__DOT__hard_stop) {
                vlSelfRef.agnus__DOT__bpd1__DOT__hardena = 0U;
            }
            if ((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__soft_start) 
                  & ((IData)(vlSelfRef.ecs) | ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__vdiwena) 
                                               & (IData)(vlSelfRef.agnus__DOT__bplen)))) 
                 & (0x49U != (IData)(vlSelfRef.agnus__DOT__reg_address)))) {
                vlSelfRef.agnus__DOT__bpd1__DOT__softena = 1U;
            } else if (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__soft_stop) 
                        | ((~ (IData)(vlSelfRef.ecs)) 
                           & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__hard_stop)))) {
                vlSelfRef.agnus__DOT__bpd1__DOT__softena = 0U;
            }
            vlSelfRef.agnus__DOT__bpd1__DOT__hard_start 
                = (0x18U == (0xffU & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                                      >> 1U)));
            vlSelfRef.agnus__DOT__bpd1__DOT__hard_stop 
                = (0xd8U == (0xffU & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                                      >> 1U)));
            vlSelfRef.agnus__DOT__bpd1__DOT__soft_start 
                = ((0xffU & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                             >> 1U)) == ((0xfcU & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfstrt) 
                                                   << 1U)) 
                                         | (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfstrt) 
                                             & (IData)(vlSelfRef.ecs)) 
                                            << 1U)));
            vlSelfRef.agnus__DOT__bpd1__DOT__soft_stop 
                = ((0xffU & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                             >> 1U)) == ((0xfcU & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfstop) 
                                                   << 1U)) 
                                         | (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfstop) 
                                             & (IData)(vlSelfRef.ecs)) 
                                            << 1U)));
        }
        if (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__next_word) 
             & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable))) {
            vlSelfRef.agnus__DOT__bl1__DOT__first_word_del 
                = vlSelfRef.agnus__DOT__bl1__DOT__first_word;
            vlSelfRef.agnus__DOT__bl1__DOT__last_word_del 
                = (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt));
        }
        if ((0x84U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod 
                = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                              >> 1U));
        }
        if ((0x85U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod 
                = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                              >> 1U));
        }
        if (vlSelfRef.agnus__DOT__cp1__DOT__enable) {
            vlSelfRef.agnus__DOT__cp1__DOT__skip_flag 
                = vlSelfRef.agnus__DOT__cp1__DOT__skip;
        }
        if (vlSelfRef.agnus__DOT__bl1__DOT__store_result) {
            vlSelfRef.agnus__DOT__bl1__DOT__bltdhold 
                = vlSelfRef.agnus__DOT__bl1__DOT__fill_out;
            vlSelfRef.agnus__DOT__bl1__DOT__fcy = (1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                                      >> 0xfU));
        }
        if ((0x38U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__bl1__DOT__bltcdat 
                = vlSelfRef.data_in;
        }
        if (vlSelfRef.agnus__DOT__bl1__DOT__enable) {
            vlSelfRef.agnus__DOT__bl1__DOT__bltahold 
                = (0xffffU & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)
                               ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_new) 
                                  | (IData)((vlSelfRef.agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_old 
                                             >> 0x10U)))
                               : ((IData)((vlSelfRef.agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_new 
                                           >> 0x10U)) 
                                  | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_old))));
            if ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                vlSelfRef.agnus__DOT__bl1__DOT__pipeline_full = 0U;
                vlSelfRef.agnus__DOT__bl1__DOT__bltaold = 0U;
                vlSelfRef.agnus__DOT__bl1__DOT__first_pixel = 1U;
            } else {
                if (vlSelfRef.agnus__DOT__bl1__DOT__next_word) {
                    vlSelfRef.agnus__DOT__bl1__DOT__pipeline_full = 1U;
                }
                if (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__next_word) 
                     & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) {
                    vlSelfRef.agnus__DOT__bl1__DOT__bltaold 
                        = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltadat) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltamask));
                }
                if ((0x18U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                    vlSelfRef.agnus__DOT__bl1__DOT__first_pixel 
                        = (1U & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__sign_del)));
                }
            }
        }
        if ((0x39U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__bl1__DOT__bltbold 
                = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbold_init)
                    ? 0U : (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbdat));
        }
        if (((IData)(vlSelfRef.reset) | (IData)(vlSelfRef.int3))) {
            vlSelfRef.agnus__DOT__bl1__DOT__start = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__bltbold_init = 1U;
        } else {
            if (((IData)(vlSelfRef.cck) & (IData)(vlSelfRef.agnus__DOT__blit_busy))) {
                vlSelfRef.agnus__DOT__bl1__DOT__start = 1U;
            }
            if ((0x39U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__bl1__DOT__bltbold_init = 0U;
            }
        }
        if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))) {
            vlSelfRef.agnus__DOT__bl1__DOT__bltbhold 
                = (0xffffU & (- (IData)((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__shiftbout)))));
        } else if (vlSelfRef.agnus__DOT__bl1__DOT__bltbdat_wrtn) {
            vlSelfRef.agnus__DOT__bl1__DOT__bltbhold 
                = vlSelfRef.agnus__DOT__bl1__DOT__shiftbout;
        }
        if (((IData)(vlSelfRef.agnus__DOT__dma_dsk) 
             | (0x10U == (IData)(vlSelfRef.agnus__DOT__reg_address)))) {
            vlSelfRef.agnus__DOT__address_dsk = ((0x7fffU 
                                                  & vlSelfRef.agnus__DOT__address_dsk) 
                                                 | (0xf8000U 
                                                    & vlSelfRef.agnus__DOT__dsk1__DOT__address_outnew));
        }
        if (((IData)(vlSelfRef.agnus__DOT__dma_dsk) 
             | (0x11U == (IData)(vlSelfRef.agnus__DOT__reg_address)))) {
            vlSelfRef.agnus__DOT__address_dsk = ((0xf8000U 
                                                  & vlSelfRef.agnus__DOT__address_dsk) 
                                                 | (0x7fffU 
                                                    & vlSelfRef.agnus__DOT__dsk1__DOT__address_outnew));
        }
        if (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
             & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)))) {
            vlSelfRef.agnus__DOT__blit_zero = 1U;
        } else if (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__store_result) 
                    & (0U != (IData)(vlSelfRef.agnus__DOT__bl1__DOT__fill_out)))) {
            vlSelfRef.agnus__DOT__blit_zero = 0U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__hpos) == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hbstrt))) {
            vlSelfRef.hblank = 1U;
        } else if (((IData)(vlSelfRef.agnus__DOT__hpos) 
                    == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hbstop))) {
            vlSelfRef.hblank = 0U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__hpos) == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hsstrt))) {
            vlSelfRef._hsync = 0U;
        } else if (((IData)(vlSelfRef.agnus__DOT__hpos) 
                    == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hsstop))) {
            vlSelfRef._hsync = 1U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__hpos) == 
             (0x1ffU & ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hsstrt) 
                        - ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hsstop) 
                           - (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hsstrt)))))) {
            vlSelfRef.agnus__DOT__bc1__DOT__vser = 1U;
        } else if (((IData)(vlSelfRef.agnus__DOT__hpos) 
                    == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hsstrt))) {
            vlSelfRef.agnus__DOT__bc1__DOT__vser = 0U;
        }
        if ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            vlSelfRef.agnus__DOT__bl1__DOT__enad = 1U;
        } else if (((0x20U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                    & (IData)(vlSelfRef.agnus__DOT__blit_busy))) {
            vlSelfRef.agnus__DOT__bl1__DOT__enad = 0U;
        }
        if (vlSelfRef.agnus__DOT__bc1__DOT__vpos_inc) {
            vlSelfRef.agnus__DOT__bc1__DOT__extra_line 
                = ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame) 
                   & (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vpos_equ_vtotal));
        }
        if (((IData)(vlSelfRef.agnus__DOT__hpos) != (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hbstrt))) {
            if (((IData)(vlSelfRef.agnus__DOT__hpos) 
                 == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hbstop))) {
                vlSelfRef.vblank = vlSelfRef.agnus__DOT__vbl;
            }
        }
        if ((IData)((0x31U == (IData)(vlSelfRef.agnus__DOT__hpos)))) {
            vlSelfRef.agnus__DOT__spr1__DOT__enable = 1U;
        } else if ((IData)((0x71U == (IData)(vlSelfRef.agnus__DOT__hpos)))) {
            vlSelfRef.agnus__DOT__spr1__DOT__enable = 0U;
        }
        vlSelfRef.vbl_int = ((8U == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                             & ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                == ((IData)(vlSelfRef.a1k)
                                     ? 1U : 0U)));
        if ((((((IData)(vlSelfRef.agnus__DOT__vpos) 
                == ((IData)(1U) + (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vsstrt))) 
               & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                  == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hsstrt))) 
              & (IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame)) 
             | ((((IData)(vlSelfRef.agnus__DOT__vpos) 
                  == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vsstrt)) 
                 & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                    == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hcenter))) 
                & (~ (IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame))))) {
            vlSelfRef._vsync = 0U;
        } else if ((((((IData)(vlSelfRef.agnus__DOT__vpos) 
                       == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vsstop)) 
                      & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                         == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hcenter))) 
                     & (IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame)) 
                    | ((((IData)(vlSelfRef.agnus__DOT__vpos) 
                         == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vsstop)) 
                        & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                           == (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hsstrt))) 
                       & (~ (IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame))))) {
            vlSelfRef._vsync = 1U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__hpos) == (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__cur_hdiwstrt))) {
            __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde 
                = (1U | (IData)(__Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde));
        }
        if ((1U & (((IData)(vlSelfRef.agnus__DOT__hpos) 
                    == (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__cur_hdiwstop)) 
                   | (~ (0U != (IData)(vlSelfRef.agnus__DOT__hpos)))))) {
            __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde 
                = (0x7feU & (IData)(__Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde));
        }
        __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde 
            = ((1U & (IData)(__Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde)) 
               | (0x7feU & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde) 
                            << 1U)));
        vlSelfRef.hde = (1U & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde) 
                               >> 0xaU));
        if (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copjmp2) 
             & (IData)(vlSelfRef.cck))) {
            vlSelfRef.agnus__DOT__cp1__DOT__strobe2 = 1U;
        } else if (((0U == (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)) 
                    & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack))) {
            vlSelfRef.agnus__DOT__cp1__DOT__strobe2 = 0U;
        }
        if (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copjmp1) 
             & (IData)(vlSelfRef.cck))) {
            vlSelfRef.agnus__DOT__cp1__DOT__strobe1 = 1U;
        } else if (((0U == (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)) 
                    & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack))) {
            vlSelfRef.agnus__DOT__cp1__DOT__strobe1 = 0U;
        }
        vlSelfRef.agnus__DOT__bc1__DOT__end_of_line 
            = ((IData)(vlSelfRef.agnus__DOT__hpos) 
               == (0x1feU & (IData)(vlSelfRef.htotal)));
        vlSelfRef.agnus__DOT__bl1__DOT__bltbdat_wrtn 
            = (0x39U == (IData)(vlSelfRef.agnus__DOT__reg_address));
        vlSelfRef.agnus__DOT__bl1__DOT__store_result 
            = ((~ (IData)(vlSelfRef.reset)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
                                               & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__next_word)));
        vlSelfRef.agnus__DOT__bc1__DOT__vpos_inc = 
            (2U == (IData)(vlSelfRef.agnus__DOT__hpos));
        if (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
             & (0x19U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)))) {
            vlSelfRef.agnus__DOT__bl1__DOT__sign_del 
                = vlSelfRef.agnus__DOT__bl1__DOT__sign;
        }
        if ((1U == (7U & (IData)(vlSelfRef.agnus__DOT__hpos)))) {
            vlSelfRef.agnus__DOT__spr1__DOT__sprite 
                = ((4U & (VL_REDXOR_8((0x30U & (IData)(vlSelfRef.agnus__DOT__hpos))) 
                          << 2U)) | ((2U & ((~ ((IData)(vlSelfRef.agnus__DOT__hpos) 
                                                >> 4U)) 
                                            << 1U)) 
                                     | (1U & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                                              >> 3U))));
        }
        if ((1U & (~ (0U != (IData)(vlSelfRef.agnus__DOT__hpos))))) {
            if ((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstrt) 
                  < (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstop)) 
                 & ((0x1ffU & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop) 
                               - (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt))) 
                    < (0x1ffU & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstop) 
                                 - (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstrt)))))) {
                __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt 
                    = vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstrt;
                __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop 
                    = vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstop;
            }
            if ((1U & (~ (0U != (IData)(vlSelfRef.agnus__DOT__vpos))))) {
                if (((0U != (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt)) 
                     | (0U != (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop)))) {
                    vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__cur_hdiwstrt 
                        = vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt;
                    vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__cur_hdiwstop 
                        = vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop;
                }
                __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt = 0U;
                __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop = 0U;
            }
        }
        if (vlSelfRef.cck) {
            vlSelfRef.agnus__DOT__cp1__DOT__bus_ena 
                = (1U & ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__bus_blk) 
                         | (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__bus_ena))));
            vlSelfRef.agnus__DOT__cp1__DOT__beam_match_wait 
                = vlSelfRef.agnus__DOT__cp1__DOT__beam_match_skip;
            vlSelfRef.agnus__DOT__cp1__DOT__beam_match 
                = (((0x7f80U & ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                << 7U)) | (0x7fU & 
                                           ((IData)(vlSelfRef.agnus__DOT__hpos) 
                                            >> 2U))) 
                   >= (((((0x4000U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1)) 
                          | (0x2000U & (((0x4000U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                          ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                             >> 0xdU)
                                          : ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                             >> 6U)) 
                                        << 0xdU))) 
                         | ((0x1000U & (((0x2000U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                          ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                             >> 0xcU)
                                          : ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                             >> 5U)) 
                                        << 0xcU)) | 
                            (0x800U & (((0x1000U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                         ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                            >> 0xbU)
                                         : ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                            >> 4U)) 
                                       << 0xbU)))) 
                        | (((0x400U & (((0x800U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                         ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                            >> 0xaU)
                                         : ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                            >> 3U)) 
                                       << 0xaU)) | 
                            (0x200U & (((0x400U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                         ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                            >> 9U) : 
                                        ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                         >> 2U)) << 9U))) 
                           | ((0x100U & (((0x200U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                           ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                              >> 8U)
                                           : ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                              >> 1U)) 
                                         << 8U)) | 
                              (0x80U & (((0x100U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                          ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                             >> 7U)
                                          : (IData)(vlSelfRef.agnus__DOT__vpos)) 
                                        << 7U))))) 
                       | ((((0x40U & (((0x80U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                        ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                           >> 6U) : 
                                       ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                        >> 7U)) << 6U)) 
                            | (0x20U & (((0x40U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                          ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                             >> 5U)
                                          : ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                             >> 6U)) 
                                        << 5U))) | 
                           ((0x10U & (((0x20U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                        ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                           >> 4U) : 
                                       ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                        >> 5U)) << 4U)) 
                            | (8U & (((0x10U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                       ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                          >> 3U) : 
                                      ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                       >> 4U)) << 3U)))) 
                          | ((4U & (((8U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                      ? ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                         >> 2U) : ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                                   >> 3U)) 
                                    << 2U)) | ((2U 
                                                & (((4U 
                                                     & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                                     ? 
                                                    ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1) 
                                                     >> 1U)
                                                     : 
                                                    ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                                     >> 2U)) 
                                                   << 1U)) 
                                               | (1U 
                                                  & ((2U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                                      ? (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir1)
                                                      : 
                                                     ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                                      >> 1U))))))));
        }
        if ((0x46U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__cp1__DOT__ir1 = (0x7fffU 
                                                   & ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2) 
                                                      >> 1U));
            vlSelfRef.agnus__DOT__cp1__DOT__ir2 = vlSelfRef.data_in;
        }
        if (((IData)(vlSelfRef.reset) | ((IData)(vlSelfRef.cck) 
                                         & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__strobe)))) {
            vlSelfRef.agnus__DOT__cp1__DOT__copper_state = 0U;
        } else if (vlSelfRef.agnus__DOT__cp1__DOT__enable) {
            vlSelfRef.agnus__DOT__cp1__DOT__copper_state 
                = vlSelfRef.agnus__DOT__cp1__DOT__copper_next;
        }
        if (vlSelfRef.cck) {
            vlSelfRef.agnus__DOT__cp1__DOT__bus_blk 
                = (1U == (0xffU & ((IData)(vlSelfRef.agnus__DOT__hpos) 
                                   >> 1U)));
            vlSelfRef.agnus__DOT__cp1__DOT__strobe 
                = ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copjmp1) 
                   | (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copjmp2));
        }
        if ((0x21U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__bl1__DOT__sign = 
                (1U & ((IData)(vlSelfRef.data_in) >> 6U));
        } else if (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
                    & (0x19U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)))) {
            vlSelfRef.agnus__DOT__bl1__DOT__sign = 
                (1U & (vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__newptr 
                       >> 0xeU));
        }
        if (vlSelfRef.reset) {
            __Vdly__agnus__DOT__bc1__DOT__long_frame = 1U;
            __Vdly__agnus__DOT__bl1__DOT__bltcon1 = 
                (0xf000U & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon1));
            __Vdly__agnus__DOT__bl1__DOT__bltcon1 = 
                (0xfffU & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon1));
            __Vdly__agnus__DOT__dmacon = 0U;
            __Vdly__agnus__DOT__bl1__DOT__bltcon0 = 
                (0xf0ffU & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon0));
            __Vdly__agnus__DOT__bl1__DOT__bltcon0 = 
                (0xff00U & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon0));
            __Vdly__agnus__DOT__bl1__DOT__bltcon0 = 
                (0xfffU & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon0));
            vlSelfRef.agnus__DOT__bc1__DOT__hbstop_reg = 0x66U;
            vlSelfRef.agnus__DOT__bc1__DOT__vsstop_reg = 5U;
            vlSelfRef.agnus__DOT__bc1__DOT__hbstrt_reg = 0x19U;
            vlSelfRef.agnus__DOT__bc1__DOT__hsstop_reg = 0x46U;
            vlSelfRef.agnus__DOT__bc1__DOT__hsstrt_reg = 0x25U;
            vlSelfRef.agnus__DOT__bc1__DOT__htotal_reg = 0x1c4U;
            vlSelfRef.agnus__DOT__bc1__DOT__vsstrt_reg = 2U;
            vlSelfRef.agnus__DOT__bc1__DOT__hcenter_reg = 0x108U;
            vlSelfRef.agnus__DOT__bl1__DOT__bltafwm = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__bltalwm = 0U;
            vlSelfRef.agnus__DOT__cp1__DOT__cdang = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel = 0U;
            if (vlSelfRef.agnus__DOT__bc1__DOT__pal) {
                vlSelfRef.agnus__DOT__bc1__DOT__vtotal_reg = 0x137U;
                vlSelfRef.agnus__DOT__bc1__DOT__vbstop_reg = 0x19U;
            } else {
                vlSelfRef.agnus__DOT__bc1__DOT__vtotal_reg = 0x105U;
                vlSelfRef.agnus__DOT__bc1__DOT__vbstop_reg = 0x14U;
            }
            vlSelfRef.agnus__DOT__bpd1__DOT__fmode = 0U;
            vlSelfRef.lace = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__height = 0U;
            vlSelfRef.agnus__DOT__bc1__DOT__ersy = 0U;
            vlSelfRef.agnus__DOT__cp1__DOT__cop1lch = 0U;
            vlSelfRef.agnus__DOT__cp1__DOT__cop1lcl = 0U;
            vlSelfRef.agnus__DOT__cp1__DOT__cop2lch = 0U;
            vlSelfRef.agnus__DOT__cp1__DOT__cop2lcl = 0U;
            vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg 
                = (0x20U & ((~ (IData)(vlSelfRef.ntsc)) 
                            << 5U));
            vlSelfRef.agnus__DOT__bl1__DOT__bltbdat = 0U;
            vlSelfRef.agnus__DOT__blit_busy = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__bltadat = 0U;
            vlSelfRef.agnus__DOT__bc1__DOT__pal = (1U 
                                                   & (~ (IData)(vlSelfRef.ntsc)));
            vlSelfRef.agnus__DOT__spr1__DOT__fmode = 0U;
            vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0 = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__blt_state = 0U;
        } else {
            if ((0x15U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                __Vdly__agnus__DOT__bc1__DOT__long_frame 
                    = (1U & ((IData)(vlSelfRef.data_in) 
                             >> 0xfU));
            } else if (((IData)(vlSelfRef.sof) & (IData)(vlSelfRef.lace))) {
                __Vdly__agnus__DOT__bc1__DOT__long_frame 
                    = (1U & (~ (IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame)));
            }
            if ((0x21U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                __Vdly__agnus__DOT__bl1__DOT__bltcon1 
                    = ((0xf000U & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon1)) 
                       | (0xfffU & (IData)(vlSelfRef.data_in)));
            }
            if (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
                 & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__decbsh))) {
                __Vdly__agnus__DOT__bl1__DOT__bltcon1 
                    = ((0xfffU & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon1)) 
                       | (0xf000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                                       >> 0xcU) - (IData)(1U)) 
                                     << 0xcU)));
            } else if ((0x21U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                __Vdly__agnus__DOT__bl1__DOT__bltcon1 
                    = ((0xfffU & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon1)) 
                       | (0xf000U & (IData)(vlSelfRef.data_in)));
            }
            if ((0x4bU == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                __Vdly__agnus__DOT__dmacon = (0x1fffU 
                                              & ((0x8000U 
                                                  & (IData)(vlSelfRef.data_in))
                                                  ? 
                                                 ((IData)(vlSelfRef.agnus__DOT__dmacon) 
                                                  | (IData)(vlSelfRef.data_in))
                                                  : 
                                                 ((IData)(vlSelfRef.agnus__DOT__dmacon) 
                                                  & (~ (IData)(vlSelfRef.data_in)))));
            }
            if ((0x20U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                __Vdly__agnus__DOT__bl1__DOT__bltcon0 
                    = ((0xf0ffU & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon0)) 
                       | (0xf00U & (IData)(vlSelfRef.data_in)));
            }
            if (((0x20U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                 | ((0x2dU == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                    & (IData)(vlSelfRef.ecs)))) {
                __Vdly__agnus__DOT__bl1__DOT__bltcon0 
                    = ((0xff00U & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon0)) 
                       | (0xffU & (IData)(vlSelfRef.data_in)));
            }
            if (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
                 & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__decbsh) 
                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_7)))) {
                __Vdly__agnus__DOT__bl1__DOT__bltcon0 
                    = ((0xfffU & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon0)) 
                       | (0xf000U & (((IData)(1U) + 
                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                       >> 0xcU)) << 0xcU)));
            } else if (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__decbsh) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_8)))) {
                __Vdly__agnus__DOT__bl1__DOT__bltcon0 
                    = ((0xfffU & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon0)) 
                       | (0xf000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                       >> 0xcU) - (IData)(1U)) 
                                     << 0xcU)));
            } else if ((0x20U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                __Vdly__agnus__DOT__bl1__DOT__bltcon0 
                    = ((0xfffU & (IData)(__Vdly__agnus__DOT__bl1__DOT__bltcon0)) 
                       | (0xf000U & (IData)(vlSelfRef.data_in)));
            }
            if (((((((((0xe0U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                       | (0xefU == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                      | (0xe1U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                     | (0xf1U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                    | (0xe2U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                   | (0xe3U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                  | (0xe4U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                 | (0xf0U == (IData)(vlSelfRef.agnus__DOT__reg_address)))) {
                if ((0xe0U != (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                    if ((0xefU != (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                        if ((0xe1U != (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                            if ((0xf1U != (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                                if ((0xe2U != (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                                    if ((0xe3U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                                        vlSelfRef.agnus__DOT__bc1__DOT__hbstop_reg 
                                            = (0x1feU 
                                               & ((IData)(vlSelfRef.data_in) 
                                                  << 1U));
                                    }
                                    if ((0xe3U != (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                                        if ((0xe4U 
                                             != (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                                            vlSelfRef.agnus__DOT__bc1__DOT__vsstrt_reg 
                                                = (0x7ffU 
                                                   & (IData)(vlSelfRef.data_in));
                                        }
                                        if ((0xe4U 
                                             == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                                            vlSelfRef.agnus__DOT__bc1__DOT__vtotal_reg 
                                                = (0x7ffU 
                                                   & (IData)(vlSelfRef.data_in));
                                        }
                                    }
                                }
                                if ((0xe2U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                                    vlSelfRef.agnus__DOT__bc1__DOT__hbstrt_reg 
                                        = (0x1feU & 
                                           ((IData)(vlSelfRef.data_in) 
                                            << 1U));
                                }
                            }
                            if ((0xf1U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                                vlSelfRef.agnus__DOT__bc1__DOT__hcenter_reg 
                                    = (0x1feU & ((IData)(vlSelfRef.data_in) 
                                                 << 1U));
                            }
                        }
                        if ((0xe1U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                            vlSelfRef.agnus__DOT__bc1__DOT__hsstop_reg 
                                = (0x1feU & ((IData)(vlSelfRef.data_in) 
                                             << 1U));
                        }
                    }
                    if ((0xefU == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                        vlSelfRef.agnus__DOT__bc1__DOT__hsstrt_reg 
                            = (0x1feU & ((IData)(vlSelfRef.data_in) 
                                         << 1U));
                    }
                }
                if ((0xe0U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                    vlSelfRef.agnus__DOT__bc1__DOT__htotal_reg 
                        = (0x1feU & ((IData)(vlSelfRef.data_in) 
                                     << 1U));
                }
            }
            if ((1U & (~ ((((((((0xe0U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                                | (0xefU == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                               | (0xe1U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                              | (0xf1U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                             | (0xe2U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                            | (0xe3U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                           | (0xe4U == (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                          | (0xf0U == (IData)(vlSelfRef.agnus__DOT__reg_address)))))) {
                if ((0xe5U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                    vlSelfRef.agnus__DOT__bc1__DOT__vsstop_reg 
                        = (0x7ffU & (IData)(vlSelfRef.data_in));
                }
                if ((0xe5U != (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                    if ((0xe7U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                        vlSelfRef.agnus__DOT__bc1__DOT__vbstop_reg 
                            = (0x7ffU & (IData)(vlSelfRef.data_in));
                    }
                }
            }
            if ((0x22U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__bl1__DOT__bltafwm 
                    = vlSelfRef.data_in;
            }
            if ((0x23U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__bl1__DOT__bltalwm 
                    = vlSelfRef.data_in;
            }
            if ((0x17U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__cp1__DOT__cdang 
                    = (1U & ((IData)(vlSelfRef.data_in) 
                             >> 1U));
            }
            if (vlSelfRef.agnus__DOT__bl1__DOT__enable) {
                if ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                    vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel = 1U;
                } else if ((0x18U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                    vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel = 0U;
                }
            }
            if (((IData)(vlSelfRef.aga) & (0xfeU == (IData)(vlSelfRef.agnus__DOT__reg_address)))) {
                vlSelfRef.agnus__DOT__bpd1__DOT__fmode 
                    = vlSelfRef.data_in;
                vlSelfRef.agnus__DOT__spr1__DOT__fmode 
                    = vlSelfRef.data_in;
            }
            if ((0x80U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.lace = (1U & ((IData)(vlSelfRef.data_in) 
                                        >> 2U));
                vlSelfRef.agnus__DOT__bc1__DOT__ersy 
                    = (1U & ((IData)(vlSelfRef.data_in) 
                             >> 1U));
                vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0 
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
            if ((0x2eU == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__bl1__DOT__height 
                    = (0x7fffU & (IData)(vlSelfRef.data_in));
            }
            if ((0x40U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__cp1__DOT__cop1lch 
                    = (0x1fU & (IData)(vlSelfRef.data_in));
            }
            if ((0x41U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__cp1__DOT__cop1lcl 
                    = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                                  >> 1U));
            }
            if ((0x42U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__cp1__DOT__cop2lch 
                    = (0x1fU & (IData)(vlSelfRef.data_in));
            }
            if ((0x43U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__cp1__DOT__cop2lcl 
                    = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                                  >> 1U));
            }
            if (((0xeeU == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                 & (IData)(vlSelfRef.ecs))) {
                vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg 
                    = vlSelfRef.data_in;
                vlSelfRef.agnus__DOT__bc1__DOT__pal 
                    = (1U & ((IData)(vlSelfRef.data_in) 
                             >> 5U));
            }
            if ((0x39U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__bl1__DOT__bltbdat 
                    = vlSelfRef.data_in;
            }
            if (((0x2cU == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                 | ((0x2fU == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                    & (IData)(vlSelfRef.ecs)))) {
                vlSelfRef.agnus__DOT__blit_busy = 1U;
            } else if (vlSelfRef.int3) {
                vlSelfRef.agnus__DOT__blit_busy = 0U;
            }
            if ((0x3aU == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
                vlSelfRef.agnus__DOT__bl1__DOT__bltadat 
                    = vlSelfRef.data_in;
            }
            vlSelfRef.agnus__DOT__bl1__DOT__blt_state 
                = vlSelfRef.agnus__DOT__bl1__DOT__blt_next;
        }
        if ((0x45U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__cp1__DOT__copjmp2 = 1U;
        } else if (vlSelfRef.cck) {
            vlSelfRef.agnus__DOT__cp1__DOT__copjmp2 = 0U;
        }
        if (((0x44U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
             | (IData)(vlSelfRef.sof))) {
            vlSelfRef.agnus__DOT__cp1__DOT__copjmp1 = 1U;
        } else if (vlSelfRef.cck) {
            vlSelfRef.agnus__DOT__cp1__DOT__copjmp1 = 0U;
        }
        if (((((IData)(vlSelfRef.sof) & (~ (IData)(vlSelfRef.a1k))) 
              | ((0U == (IData)(vlSelfRef.agnus__DOT__vpos)) 
                 & (IData)(vlSelfRef.a1k))) | ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                               == (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstop)))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__vdiwena = 0U;
        } else if (((IData)(vlSelfRef.agnus__DOT__vpos) 
                    == (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstrt))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__vdiwena = 1U;
        }
        if ((0x47U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstrt 
                = (0xffU & (IData)(vlSelfRef.data_in));
            vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstrt 
                = (0xffU & ((IData)(vlSelfRef.data_in) 
                            >> 8U));
        } else if (((0xf2U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                    & (IData)(vlSelfRef.ecs))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstrt 
                = ((0xffU & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstrt)) 
                   | (0x100U & ((IData)(vlSelfRef.data_in) 
                                << 3U)));
            vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstrt 
                = ((0xffU & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstrt)) 
                   | (0x700U & ((IData)(vlSelfRef.data_in) 
                                << 8U)));
        }
        if ((0x48U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstop 
                = (0x100U | (0xffU & (IData)(vlSelfRef.data_in)));
            vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstop 
                = ((0x100U & ((~ ((IData)(vlSelfRef.data_in) 
                                  >> 0xfU)) << 8U)) 
                   | (0xffU & ((IData)(vlSelfRef.data_in) 
                               >> 8U)));
        } else if (((0xf2U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                    & (IData)(vlSelfRef.ecs))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstop 
                = ((0xffU & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__hdiwstop)) 
                   | (0x100U & ((IData)(vlSelfRef.data_in) 
                                >> 5U)));
            vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstop 
                = ((0xffU & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstop)) 
                   | (0x700U & (IData)(vlSelfRef.data_in)));
        }
        if ((0x49U == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__ddfstrt 
                = (0x7fU & ((IData)(vlSelfRef.data_in) 
                            >> 1U));
        }
        if ((0x4aU == (IData)(vlSelfRef.agnus__DOT__reg_address))) {
            vlSelfRef.agnus__DOT__bpd1__DOT__ddfstop 
                = (0x7fU & ((IData)(vlSelfRef.data_in) 
                            >> 1U));
        }
    }
    if (__VdlySet__agnus__DOT__spr1__DOT__sprpos__v0) {
        vlSelfRef.agnus__DOT__spr1__DOT__sprpos[__VdlyDim0__agnus__DOT__spr1__DOT__sprpos__v0] 
            = __VdlyVal__agnus__DOT__spr1__DOT__sprpos__v0;
    }
    if (__VdlySet__agnus__DOT__spr1__DOT__sprpth__v0) {
        vlSelfRef.agnus__DOT__spr1__DOT__sprpth[__VdlyDim0__agnus__DOT__spr1__DOT__sprpth__v0] 
            = __VdlyVal__agnus__DOT__spr1__DOT__sprpth__v0;
    }
    if (__VdlySet__agnus__DOT__spr1__DOT__sprptl__v0) {
        vlSelfRef.agnus__DOT__spr1__DOT__sprptl[__VdlyDim0__agnus__DOT__spr1__DOT__sprptl__v0] 
            = __VdlyVal__agnus__DOT__spr1__DOT__sprptl__v0;
    }
    if (__VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0) {
        vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod[__VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0] 
            = __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod__v0;
    }
    if (__VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0) {
        vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth[__VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0] 
            = __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth__v0;
    }
    if (__VdlySet__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0) {
        vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl[__VdlyDim0__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0] 
            = __VdlyVal__agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl__v0;
    }
    if (__VdlySet__agnus__DOT__spr1__DOT__sprctl__v0) {
        vlSelfRef.agnus__DOT__spr1__DOT__sprctl[__VdlyDim0__agnus__DOT__spr1__DOT__sprctl__v0] 
            = __VdlyVal__agnus__DOT__spr1__DOT__sprctl__v0;
    }
    if (__VdlySet__agnus__DOT__aud1__DOT__audlch__v0) {
        vlSelfRef.agnus__DOT__aud1__DOT__audlch[__VdlyDim0__agnus__DOT__aud1__DOT__audlch__v0] 
            = __VdlyVal__agnus__DOT__aud1__DOT__audlch__v0;
    }
    if (__VdlySet__agnus__DOT__aud1__DOT__audlcl__v0) {
        vlSelfRef.agnus__DOT__aud1__DOT__audlcl[__VdlyDim0__agnus__DOT__aud1__DOT__audlcl__v0] 
            = __VdlyVal__agnus__DOT__aud1__DOT__audlcl__v0;
    }
    if (__VdlySet__agnus__DOT__aud1__DOT__audpt__v0) {
        vlSelfRef.agnus__DOT__aud1__DOT__audpt[__VdlyDim0__agnus__DOT__aud1__DOT__audpt__v0] 
            = __VdlyVal__agnus__DOT__aud1__DOT__audpt__v0;
    }
    if (__VdlySet__agnus__DOT__bpd1__DOT__bplpth__v0) {
        vlSelfRef.agnus__DOT__bpd1__DOT__bplpth[__VdlyDim0__agnus__DOT__bpd1__DOT__bplpth__v0] 
            = __VdlyVal__agnus__DOT__bpd1__DOT__bplpth__v0;
    }
    if (__VdlySet__agnus__DOT__bpd1__DOT__bplptl__v0) {
        vlSelfRef.agnus__DOT__bpd1__DOT__bplptl[__VdlyDim0__agnus__DOT__bpd1__DOT__bplptl__v0] 
            = __VdlyVal__agnus__DOT__bpd1__DOT__bplptl__v0;
    }
    vlSelfRef.agnus__DOT__bl1__DOT__height_cnt = __Vdly__agnus__DOT__bl1__DOT__height_cnt;
    vlSelfRef.agnus__DOT__bls_cnt = __Vdly__agnus__DOT__bls_cnt;
    vlSelfRef.agnus__DOT__dmacon = __Vdly__agnus__DOT__dmacon;
    vlSelfRef.agnus__DOT__bl1__DOT__bltcon0 = __Vdly__agnus__DOT__bl1__DOT__bltcon0;
    vlSelfRef.agnus__DOT__address_cop = __Vdly__agnus__DOT__address_cop;
    vlSelfRef.agnus__DOT__bc1__DOT__long_line = __Vdly__agnus__DOT__bc1__DOT__long_line;
    vlSelfRef.agnus__DOT__bl1__DOT__width_cnt = __Vdly__agnus__DOT__bl1__DOT__width_cnt;
    vlSelfRef.agnus__DOT__bl1__DOT__bltcon1 = __Vdly__agnus__DOT__bl1__DOT__bltcon1;
    vlSelfRef.agnus__DOT__bc1__DOT__long_frame = __Vdly__agnus__DOT__bc1__DOT__long_frame;
    if (__VdlySet__agnus__DOT__spr1__DOT__sprposh__v0) {
        vlSelfRef.agnus__DOT__spr1__DOT__sprposh[__VdlyDim0__agnus__DOT__spr1__DOT__sprposh__v0] 
            = __VdlyVal__agnus__DOT__spr1__DOT__sprposh__v0;
    }
    vlSelfRef.agnus__DOT__spr1__DOT__dmastate_mem[__VdlyDim0__agnus__DOT__spr1__DOT__dmastate_mem__v0] 
        = __VdlyVal__agnus__DOT__spr1__DOT__dmastate_mem__v0;
    vlSelfRef.agnus__DOT__spr1__DOT__sprsel = __Vdly__agnus__DOT__spr1__DOT__sprsel;
    vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde 
        = __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde;
    vlSelfRef.agnus__DOT__bc1__DOT__hpos_r = __Vdly__agnus__DOT__bc1__DOT__hpos_r;
    if (__VdlySet__agnus__DOT__bpd1__DOT__bplcon0_delay__v0) {
        vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delay[0U] 
            = __VdlyVal__agnus__DOT__bpd1__DOT__bplcon0_delay__v0;
        vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delay[1U] 
            = __VdlyVal__agnus__DOT__bpd1__DOT__bplcon0_delay__v1;
    }
    vlSelfRef.agnus__DOT__blten = (IData)((0x240U == 
                                           (0x240U 
                                            & (IData)(vlSelfRef.agnus__DOT__dmacon))));
    agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0 
        = ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__bus_ena) 
           & (IData)(vlSelfRef.cck));
    vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14 
        = ((4U & ((~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                  << 2U)) | ((2U & ((~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                        >> 1U)) << 1U)) 
                             | (1U & (~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                         >> 2U)))));
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_12 
        = ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt)) 
           & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__height_cnt)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x8000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xfU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xfU))) 
                           & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xfU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                         << 0xfU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x4000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xeU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xeU))) 
                           & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xeU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                         << 0xeU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x2000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xdU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xdU))) 
                           & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xdU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                         << 0xdU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x1000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xcU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xcU))) 
                           & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xcU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                         << 0xcU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x800U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xbU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                               >> 0xbU))) 
                          & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xbU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                        << 0xbU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x400U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xaU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                               >> 0xaU))) 
                          & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xaU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                        << 0xaU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x200U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 9U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 9U))) 
                          & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 9U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                        << 9U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x100U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 8U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 8U))) 
                          & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 8U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                        << 8U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x80U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 7U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 7U))) 
                         & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 7U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                       << 7U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x40U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 6U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 6U))) 
                         & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 6U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                       << 6U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x20U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 5U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 5U))) 
                         & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 5U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                       << 5U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x10U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 4U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 4U))) 
                         & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 4U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                       << 4U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (8U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 3U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 3U))) & 
                      (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                          >> 3U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                    << 3U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (4U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 2U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 2U))) & 
                      (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                          >> 2U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                    << 2U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (2U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 1U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 1U))) & 
                      (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                          >> 1U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                    << 1U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (1U & ((((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold)) 
                      & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold))) 
                     & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat))) 
                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | ((IData)(((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                             >> 0xfU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 0xfU))) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                           >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                        >> 1U))) << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x4000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xeU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xeU))) 
                           << 0xeU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xdU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x2000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xdU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xdU))) 
                           << 0xdU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xcU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x1000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xcU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xcU))) 
                           << 0xcU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xbU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x800U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xbU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                               >> 0xbU))) 
                          << 0xbU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 0xaU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x400U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xaU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                               >> 0xaU))) 
                          << 0xaU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x200U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 9U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 9U))) 
                          << 9U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x100U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 8U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 8U))) 
                          << 8U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x80U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 7U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 7U))) 
                         << 7U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x40U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 6U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 6U))) 
                         << 6U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x20U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 5U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 5U))) 
                         << 5U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x10U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 4U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 4U))) 
                         << 4U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (8U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 3U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 3U))) << 3U) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (4U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 2U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 2U))) << 2U) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (2U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 1U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 1U))) << 1U) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (1U & ((((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold)) 
                      & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold))) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | ((IData)(((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                             >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                          >> 0xfU)) 
                        & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                              >> 0xfU))) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                            >> 2U))) 
              << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x4000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xeU)) << 0xeU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xeU)) << 0xeU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xcU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x2000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xdU)) << 0xdU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xdU)) << 0xdU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xbU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x1000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xcU)) << 0xcU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xcU)) << 0xcU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xaU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x800U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xbU)) << 0xbU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xbU)) << 0xbU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x400U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xaU)) << 0xaU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xaU)) << 0xaU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x200U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 9U)) << 9U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 9U)) << 9U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                   << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x100U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 8U)) << 8U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 8U)) << 8U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                   << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x80U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 7U)) << 7U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 7U)) << 7U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x40U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 6U)) << 6U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 6U)) << 6U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x20U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 5U)) << 5U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 5U)) << 5U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x10U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 4U)) << 4U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 4U)) << 4U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (8U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 3U)) << 3U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 3U)) << 3U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (4U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 2U)) << 2U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 2U)) << 2U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (2U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 1U)) << 1U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 1U)) << 1U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (1U & ((((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold)) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat))) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | ((IData)(((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                             >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                          >> 0xfU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                           >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                        >> 3U))) << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x4000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xeU)) << 0xeU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xbU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x2000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xdU)) << 0xdU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xaU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x1000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xcU)) << 0xcU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x800U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xbU)) << 0xbU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x400U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xaU)) << 0xaU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x200U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 9U)) << 9U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x100U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 8U)) << 8U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x80U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 7U)) << 7U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x40U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 6U)) << 6U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x20U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 5U)) << 5U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x10U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 4U)) << 4U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (8U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 3U)) << 3U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (4U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 2U)) << 2U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (2U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 1U)) << 1U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (1U & ((((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold)) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | ((IData)((((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          >> 0xfU) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 0xfU))) 
                        & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                              >> 0xfU))) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                            >> 4U))) 
              << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x4000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xeU)) << 0xeU)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xeU)) << 0xeU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xaU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x2000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xdU)) << 0xdU)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xdU)) << 0xdU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x1000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xcU)) << 0xcU)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xcU)) << 0xcU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x800U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 0xbU)) << 0xbU)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xbU)) << 0xbU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x400U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 0xaU)) << 0xaU)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xaU)) << 0xaU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x200U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 9U)) << 9U)) & 
                         ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                              >> 9U)) << 9U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                 << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x100U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 8U)) << 8U)) & 
                         ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                              >> 8U)) << 8U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                 << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x80U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 7U)) << 7U)) & (
                                                   (~ 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                     >> 7U)) 
                                                   << 7U)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x40U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 6U)) << 6U)) & (
                                                   (~ 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                     >> 6U)) 
                                                   << 6U)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x20U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 5U)) << 5U)) & (
                                                   (~ 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                     >> 5U)) 
                                                   << 5U)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x10U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 4U)) << 4U)) & (
                                                   (~ 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                     >> 4U)) 
                                                   << 4U)) 
                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (8U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 3U)) << 3U)) & ((~ 
                                                 ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                  >> 3U)) 
                                                << 3U)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (4U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 2U)) << 2U)) & ((~ 
                                                 ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                  >> 2U)) 
                                                << 2U)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (2U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 1U)) << 1U)) & ((~ 
                                                 ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                  >> 1U)) 
                                                << 1U)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (1U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold))) 
                     & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat))) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | ((IData)((((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          >> 0xfU) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 0xfU))) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                           >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                        >> 5U))) << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x4000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xeU)) << 0xeU)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x2000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xdU)) << 0xdU)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x1000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xcU)) << 0xcU)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x800U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 0xbU)) << 0xbU)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x400U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 0xaU)) << 0xaU)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x200U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 9U)) << 9U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x100U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 8U)) << 8U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x80U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 7U)) << 7U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x40U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 6U)) << 6U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x20U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 5U)) << 5U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x10U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 4U)) << 4U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (8U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 3U)) << 3U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (4U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 2U)) << 2U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (2U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 1U)) << 1U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (1U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold))) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x8000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xfU)) << 0xfU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x4000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xeU)) << 0xeU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x2000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xdU)) << 0xdU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x1000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xcU)) << 0xcU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x800U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xbU)) << 0xbU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x400U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xaU)) << 0xaU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x200U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 9U)) << 9U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                   << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x100U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 8U)) << 8U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                   << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x80U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 7U)) << 7U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x40U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 6U)) << 6U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x20U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 5U)) << 5U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x10U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 4U)) << 4U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (8U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 3U)) << 3U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (4U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 2U)) << 2U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (2U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 1U)) << 1U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (1U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat))) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x8000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x4000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x2000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x1000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x800U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x400U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x200U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x100U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x80U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x40U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x20U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x10U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (8U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (4U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (2U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (1U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__used = (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                             >> 8U) 
                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enad));
    vlSelfRef.agnus__DOT__bl1__DOT__ife = (IData)((8U 
                                                   == 
                                                   (9U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))));
    vlSelfRef.agnus__DOT__bl1__DOT__efe = (IData)((0x10U 
                                                   == 
                                                   (0x11U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))));
    vlSelfRef.agnus__DOT__bl1__DOT__desc = (IData)(
                                                   (2U 
                                                    == 
                                                    (3U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))));
    vlSelfRef._csync = (((IData)(vlSelfRef._hsync) 
                         & (IData)(vlSelfRef._vsync)) 
                        | (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vser));
    vlSelfRef.field1 = (1U & (~ (IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame)));
    vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_0 
        = (1U & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                 [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                 >> 3U));
    vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_3 
        = ((0x100U & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                      [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                      << 7U)) | vlSelfRef.agnus__DOT__spr1__DOT__sprpos
           [vlSelfRef.agnus__DOT__spr1__DOT__sprsel]);
    vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_1 
        = (1U & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                 [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                 >> 2U));
    vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_4 
        = ((0x100U & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                      [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                      << 8U)) | (0xffU & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                                          [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                                          >> 4U)));
    vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17 
        = ((3U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))
            ? 4U : ((0U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))
                     ? 1U : 2U));
    vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12 = (
                                                   (1U 
                                                    == 
                                                    (3U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))) 
                                                   | (2U 
                                                      == 
                                                      (3U 
                                                       & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))));
    agnus__DOT__bpd1__DOT__shres = ((IData)(vlSelfRef.ecs) 
                                    & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
                                       >> 5U));
    vlSelfRef.agnus__DOT__bpd1__DOT__ddfend = __Vdly__agnus__DOT__bpd1__DOT__ddfend;
    vlSelfRef.agnus__DOT__bpd1__DOT__dmaena_delayed 
        = __Vdly__agnus__DOT__bpd1__DOT__dmaena_delayed;
    vlSelfRef.agnus__DOT__bl1__DOT__minterm_out = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0) 
                                                   | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1) 
                                                      | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2) 
                                                         | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3) 
                                                            | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4) 
                                                               | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5) 
                                                                  | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6) 
                                                                     | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7))))))));
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_11 
        = ((1U & (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                   >> 9U) | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__ife) 
                             | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__efe))))
            ? 0xaU : 8U);
    __Vtableidx6 = ((0x1eU & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                              >> 0xbU)) | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc));
    agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot 
        = Vagnus__ConstPool__TABLE_hbbf19d2e_0[__Vtableidx6];
    agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0 
        = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc) 
           & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt)));
    agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0 
        = ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)) 
           & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt)));
    __Vtableidx7 = ((0x1eU & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                              >> 0xbU)) | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc));
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot 
        = Vagnus__ConstPool__TABLE_hbbf19d2e_0[__Vtableidx7];
    vlSelfRef.agnus__DOT__spr1__DOT__vstop = (((IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_1) 
                                               << 9U) 
                                              | (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_4));
    vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt 
        = __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt;
    vlSelfRef.agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop 
        = __Vdly__agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18 
        = ((~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
               >> 4U)) & ((~ (IData)(agnus__DOT__bpd1__DOT__shres)) 
                          & (3U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7 
        = ((IData)(agnus__DOT__bpd1__DOT__shres) & 
           (0U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11 
        = ((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
             >> 4U) & (0U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))) 
           | ((IData)(agnus__DOT__bpd1__DOT__shres) 
              & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12)));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5 
        = (1U & ((~ (IData)(agnus__DOT__bpd1__DOT__shres)) 
                 & (~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
                       >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__first_word = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__width) 
                                                  == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt));
    vlSelfRef.varbeamen = (1U & ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
                                 >> 7U));
    vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_9 
        = ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfend) 
           & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
              >> 2U));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (1U & (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_word_del)
                      ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                         >> 2U) : (IData)(vlSelfRef.agnus__DOT__bl1__DOT__fcy)) 
                    ^ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out))));
    vlSelfRef.sol = vlSelfRef.agnus__DOT__bc1__DOT__vpos_inc;
    vlSelfRef.agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_old 
        = (0xfffffffffULL & ((QData)((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltaold)) 
                             * (QData)((IData)(agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot))));
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new 
        = (0xfffffffffULL & ((QData)((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbdat)) 
                             * (QData)((IData)(agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot))));
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old 
        = (0xfffffffffULL & ((QData)((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbold)) 
                             * (QData)((IData)(agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot))));
    agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14 
        = (1U & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                     >> 3U)) & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__sign_del))));
    agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13 
        = (1U & ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__sign_del)) 
                 & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                    >> 3U)));
    vlSelfRef.agnus__DOT__address_spr = ((vlSelfRef.agnus__DOT__spr1__DOT__sprpth
                                          [vlSelfRef.agnus__DOT__spr1__DOT__sprite] 
                                          << 0xfU) 
                                         | vlSelfRef.agnus__DOT__spr1__DOT__sprptl
                                         [vlSelfRef.agnus__DOT__spr1__DOT__sprite]);
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6 
        = (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
            & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12)) 
           | (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
               >> 4U) & (3U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltamask = (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_word) 
                                                 & (1U 
                                                    == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt)))
                                                 ? 
                                                ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltafwm) 
                                                 & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltalwm))
                                                 : 
                                                ((1U 
                                                  == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt))
                                                  ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltalwm)
                                                  : 
                                                 ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_word)
                                                   ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltafwm)
                                                   : 0xffffU)));
    vlSelfRef.agnus__DOT__cp1__DOT__enable = ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__bus_blk)) 
                                              & (IData)(agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0));
    if (vlSelfRef.varbeamen) {
        vlSelfRef.htotal = vlSelfRef.agnus__DOT__bc1__DOT__htotal_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__hbstrt = vlSelfRef.agnus__DOT__bc1__DOT__hbstrt_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__hbstop = vlSelfRef.agnus__DOT__bc1__DOT__hbstop_reg;
        agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_1 
            = (1U & ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
                     >> 9U));
        agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_0 
            = (1U & ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
                     >> 8U));
    } else {
        vlSelfRef.htotal = 0x1c4U;
        vlSelfRef.agnus__DOT__bc1__DOT__hbstrt = 0x19U;
        vlSelfRef.agnus__DOT__bc1__DOT__hbstop = 0x66U;
        agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_1 = 0U;
        agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_0 = 0U;
    }
    agnus__DOT__bc1__DOT__vbstop = ((((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
                                      >> 0xcU) & (IData)(vlSelfRef.varbeamen))
                                     ? (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vbstop_reg)
                                     : ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__pal)
                                         ? 0x19U : 0x14U));
    vlSelfRef.agnus__DOT__bl1__DOT__shiftbout = (0xffffU 
                                                 & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)
                                                     ? 
                                                    ((IData)(agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new) 
                                                     | (IData)(
                                                               (agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old 
                                                                >> 0x10U)))
                                                     : 
                                                    ((IData)(
                                                             (agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new 
                                                              >> 0x10U)) 
                                                     | (IData)(agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old))));
    agnus__DOT__bl1__DOT__lineadd = (1U & ((IData)(
                                                   (0U 
                                                    == 
                                                    (0x14U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                                           | (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                                               >> 4U) 
                                              & (IData)(agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14))));
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_7 
        = (1U & ((IData)((0x10U == (0x14U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                 | ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                        >> 4U)) & (IData)(agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14))));
    agnus__DOT__bl1__DOT__linesub = (1U & ((IData)(
                                                   (4U 
                                                    == 
                                                    (0x14U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                                           | (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                                               >> 4U) 
                                              & (IData)(agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13))));
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_8 
        = (1U & ((IData)((0x14U == (0x14U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                 | ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                        >> 4U)) & (IData)(agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13))));
    vlSelfRef.agnus__DOT__spr1__DOT__newptr = (0xfffffU 
                                               & (vlSelfRef.agnus__DOT__address_spr 
                                                  + 
                                                  ((0U 
                                                    == 
                                                    (3U 
                                                     & ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__fmode) 
                                                        >> 2U)))
                                                    ? 1U
                                                    : 
                                                   ((3U 
                                                     == 
                                                     (3U 
                                                      & ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__fmode) 
                                                         >> 2U)))
                                                     ? 4U
                                                     : 2U))));
    vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq_match = 
        (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18) 
          & (7U == (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq))) 
         | (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6) 
             & (7U == (0xfU & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))) 
            | ((~ (IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18)) 
               & ((~ (IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6)) 
                  & (7U == (7U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))))));
    agnus__DOT__bpd1__DOT__plane = ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                     ? (1U & (~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))
                                     : ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                         ? ((2U & (
                                                   (~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                                                   << 1U)) 
                                            | (1U & 
                                               (~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                                   >> 1U))))
                                         : ((((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
                                              & (0U 
                                                 == 
                                                 (3U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))) 
                                             | ((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
                                                  >> 4U) 
                                                 & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12)) 
                                                | ((IData)(agnus__DOT__bpd1__DOT__shres) 
                                                   & (3U 
                                                      == 
                                                      (3U 
                                                       & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))))))
                                             ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14)
                                             : ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6)
                                                 ? 
                                                ((8U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                                                 | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14))
                                                 : 
                                                ((0x18U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                                                 | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14))))));
    vlSelfRef.agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_new 
        = (0xfffffffffULL & ((QData)((IData)(((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltadat) 
                                              & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltamask)))) 
                             * (QData)((IData)(agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot))));
    if (agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_1) {
        vlSelfRef.agnus__DOT__bc1__DOT__vsstrt = vlSelfRef.agnus__DOT__bc1__DOT__vsstrt_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__vsstop = vlSelfRef.agnus__DOT__bc1__DOT__vsstop_reg;
    } else {
        vlSelfRef.agnus__DOT__bc1__DOT__vsstrt = 2U;
        vlSelfRef.agnus__DOT__bc1__DOT__vsstop = 5U;
    }
    if (agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_0) {
        vlSelfRef.agnus__DOT__bc1__DOT__hsstrt = vlSelfRef.agnus__DOT__bc1__DOT__hsstrt_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__hsstop = vlSelfRef.agnus__DOT__bc1__DOT__hsstop_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__hcenter = vlSelfRef.agnus__DOT__bc1__DOT__hcenter_reg;
    } else {
        vlSelfRef.agnus__DOT__bc1__DOT__hsstrt = 0x25U;
        vlSelfRef.agnus__DOT__bc1__DOT__hsstop = 0x46U;
        vlSelfRef.agnus__DOT__bc1__DOT__hcenter = 0x108U;
    }
    agnus__DOT__bl1__DOT__lineinc = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_7) 
                                     & (0xf000U == 
                                        (0xf000U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    agnus__DOT__bl1__DOT__linedec = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_8) 
                                     & (0U == (0xf000U 
                                               & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__cp1__DOT__beam_match_skip 
        = ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__beam_match) 
           & ((~ (IData)(vlSelfRef.agnus__DOT__blit_busy)) 
              | ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2) 
                 >> 0xfU)));
    agnus__DOT__cp1__DOT__illegalreg = (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__cdang)) 
                                         & (0U == (0x180U 
                                                   & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)))) 
                                        | ((~ (IData)(vlSelfRef.ecs)) 
                                           & (0U == 
                                              (0x1c0U 
                                               & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)))));
    agnus__DOT__address_bpl = ((vlSelfRef.agnus__DOT__bpd1__DOT__bplpth
                                [(7U & (IData)(agnus__DOT__bpd1__DOT__plane))] 
                                << 0xfU) | vlSelfRef.agnus__DOT__bpd1__DOT__bplptl
                               [(7U & (IData)(agnus__DOT__bpd1__DOT__plane))]);
    __Vtableidx4 = agnus__DOT__bpd1__DOT__plane;
    agnus__DOT__reg_address_bpl = Vagnus__ConstPool__TABLE_h7cec6eda_0
        [__Vtableidx4];
    vlSelfRef.agnus__DOT__dma_bpl = ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfrun) 
                                     & (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__dmaena_delayed) 
                                         >> 1U) & ((IData)(vlSelfRef.cck) 
                                                   & ((IData)(agnus__DOT__bpd1__DOT__plane) 
                                                      < 
                                                      (0xfU 
                                                       & ((IData)(vlSelfRef.aga)
                                                           ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed)
                                                           : 
                                                          ((7U 
                                                            == 
                                                            (7U 
                                                             & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed)))
                                                            ? 4U
                                                            : 
                                                           (7U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed)))))))));
    vlSelfRef.harddis = (1U & (IData)(((0U != (0x5000U 
                                               & (IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg))) 
                                       | (IData)(vlSelfRef.varbeamen))));
    vlSelfRef.agnus__DOT__cp1__DOT__selreg = ((1U & 
                                               ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                >> 2U)) 
                                              && ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                       >> 1U))) 
                                                  && ((1U 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)) 
                                                      && ((1U 
                                                           & (~ 
                                                              ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                               & (IData)(agnus__DOT__cp1__DOT__illegalreg)))) 
                                                          && ((1U 
                                                               & (~ 
                                                                  ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                                   & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag)))) 
                                                              && (1U 
                                                                  & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))))))));
    vlSelfRef.agnus__DOT__cp1__DOT__selins = ((1U & 
                                               ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                >> 2U)) 
                                              && ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                       >> 1U))) 
                                                  && ((1U 
                                                       & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))) 
                                                      || ((1U 
                                                           & (~ 
                                                              ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                               & (IData)(agnus__DOT__cp1__DOT__illegalreg)))) 
                                                          && (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                               & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag)) 
                                                              || (1U 
                                                                  & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)))))));
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_9 
        = (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__used)) 
            & (0xaU == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
           | (8U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)));
    agnus__DOT__cp1__DOT__dma_req = ((4U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                      ? ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                   >> 1U))) 
                                         && ((1U & 
                                              (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))) 
                                             || (1U 
                                                 & (~ 
                                                    ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                     & (IData)(agnus__DOT__cp1__DOT__illegalreg))))))
                                      : ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                   >> 1U))) 
                                         && (1U & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)))));
    agnus__DOT__dma_cop = (((IData)(agnus__DOT__cp1__DOT__dma_req) 
                            & (IData)(agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0)) 
                           & (IData)((0x280U == (0x280U 
                                                 & (IData)(vlSelfRef.agnus__DOT__dmacon)))));
    vlSelfRef.agnus__DOT__bplen = (IData)((0x300U == 
                                           (0x300U 
                                            & (IData)(vlSelfRef.agnus__DOT__dmacon))));
    if ((0x10U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__submod = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel) 
                                                            & (IData)(agnus__DOT__bl1__DOT__linesub))
                                                            : (IData)(agnus__DOT__bl1__DOT__linesub))
                                                           : 
                                                          ((1U 
                                                            & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                           && (IData)(agnus__DOT__bl1__DOT__linesub)))));
        vlSelfRef.agnus__DOT__bl1__DOT__addmod = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel) 
                                                            & (IData)(agnus__DOT__bl1__DOT__lineadd))
                                                            : (IData)(agnus__DOT__bl1__DOT__lineadd))
                                                           : 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                                           || (IData)(agnus__DOT__bl1__DOT__lineadd)))));
        vlSelfRef.agnus__DOT__bl1__DOT__modsel = ((8U 
                                                   & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                   ? 
                                                  ((4U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                    ? 0U
                                                    : 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                     ? 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                      ? 0U
                                                      : 3U)
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                      ? 
                                                     ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__sign)
                                                       ? 1U
                                                       : 2U)
                                                      : 
                                                     ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel)
                                                       ? 3U
                                                       : 0U))))
                                                   : 0U);
    } else if ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__submod = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && ((2U 
                                                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                       ? (IData)(agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0)
                                                       : 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                        ? (IData)(agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0)
                                                        : 
                                                       ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc) 
                                                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__last_word_del)))));
        vlSelfRef.agnus__DOT__bl1__DOT__addmod = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && ((2U 
                                                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                       ? (IData)(agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0)
                                                       : 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                        ? (IData)(agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0)
                                                        : 
                                                       ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)) 
                                                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__last_word_del)))));
        vlSelfRef.agnus__DOT__bl1__DOT__modsel = ((4U 
                                                   & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                   ? 0U
                                                   : 
                                                  ((2U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                    ? 
                                                   ((1U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                     ? 1U
                                                     : 0U)
                                                    : 
                                                   ((1U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                     ? 2U
                                                     : 3U)));
    } else {
        vlSelfRef.agnus__DOT__bl1__DOT__submod = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc) 
                                                              & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__last_word_del)))));
        vlSelfRef.agnus__DOT__bl1__DOT__addmod = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)) 
                                                              & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__last_word_del)))));
        vlSelfRef.agnus__DOT__bl1__DOT__modsel = ((4U 
                                                   & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                   ? 
                                                  ((2U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                    ? 0U
                                                    : 
                                                   ((1U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                     ? 0U
                                                     : 3U))
                                                   : 0U);
    }
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT____VdfgRegularize_h62043dcc_0_0 
        = ((0xf8000U & ((- (IData)((1U & (vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod
                                          [vlSelfRef.agnus__DOT__bl1__DOT__modsel] 
                                          >> 0xeU)))) 
                        << 0xfU)) | vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod
           [vlSelfRef.agnus__DOT__bl1__DOT__modsel]);
    vlSelfRef.agnus__DOT__bl1__DOT__next_word = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_9) 
                                                 | ((0x1bU 
                                                     == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                                    | (0x18U 
                                                       == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))));
    if ((0x10U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__decptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel) 
                                                            & (IData)(agnus__DOT__bl1__DOT__linedec))
                                                            : (IData)(agnus__DOT__bl1__DOT__linedec))
                                                           : 
                                                          ((1U 
                                                            & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                           && (IData)(agnus__DOT__bl1__DOT__linedec)))));
        vlSelfRef.agnus__DOT__bl1__DOT__incptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel) 
                                                            & (IData)(agnus__DOT__bl1__DOT__lineinc))
                                                            : (IData)(agnus__DOT__bl1__DOT__lineinc))
                                                           : 
                                                          ((1U 
                                                            & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                           && (IData)(agnus__DOT__bl1__DOT__lineinc)))));
        if ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            if ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
            } else if ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                    vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
                    vlSelfRef.agnus__DOT__bl1__DOT__chsel = 0U;
                } else {
                    vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 3U;
                    vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
                }
            } else if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 2U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
            } else {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel 
                    = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel)
                        ? 3U : 0U);
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 3U;
            }
        } else {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
        }
    } else if ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__decptr = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc));
        vlSelfRef.agnus__DOT__bl1__DOT__incptr = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && (1U 
                                                      & ((2U 
                                                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                          ? 
                                                         ((1U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))
                                                           : 
                                                          (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)))
                                                          : 
                                                         ((1U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))
                                                           : 
                                                          (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))))));
        if ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 
                ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 2U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? 2U : 0U));
        } else if ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 1U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 1U;
            } else {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 0U;
            }
        } else if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 2U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
        } else {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 3U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 3U;
        }
    } else {
        vlSelfRef.agnus__DOT__bl1__DOT__decptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))));
        vlSelfRef.agnus__DOT__bl1__DOT__incptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && (1U 
                                                              & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))))));
        if ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            if ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
            } else if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
            } else {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 3U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 3U;
            }
        } else {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 
                ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 2U : 0U);
        }
    }
    __Vtableidx5 = vlSelfRef.agnus__DOT__bl1__DOT__chsel;
    vlSelfRef.agnus__DOT__reg_address_blt = Vagnus__ConstPool__TABLE_h78219485_0
        [__Vtableidx5];
    vlSelfRef.agnus__DOT__vpos = __Vdly__agnus__DOT__vpos;
    vlSelfRef.agnus__DOT__hpos = (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                   << 1U) | (IData)(vlSelfRef.cck));
    vlSelfRef.agnus__DOT__bc1__DOT__vpos_equ_vtotal 
        = ((IData)(vlSelfRef.agnus__DOT__vpos) == ((IData)(vlSelfRef.varbeamen)
                                                    ? (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vtotal_reg)
                                                    : 
                                                   ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__pal)
                                                     ? 0x137U
                                                     : 0x105U)));
    vlSelfRef.agnus__DOT__vblend = ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                    == (IData)(agnus__DOT__bc1__DOT__vbstop));
    vlSelfRef.agnus__DOT__vbl = ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                 <= (IData)(agnus__DOT__bc1__DOT__vbstop));
    if ((0x25U == (IData)(vlSelfRef.agnus__DOT__hpos))) {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & (IData)(vlSelfRef.audio_dmas));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & (IData)(vlSelfRef.audio_dmal));
    } else if ((0x29U == (IData)(vlSelfRef.agnus__DOT__hpos))) {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmas) 
                                                    >> 1U));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmal) 
                                                    >> 1U));
    } else if ((0x2dU == (IData)(vlSelfRef.agnus__DOT__hpos))) {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmas) 
                                                    >> 2U));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmal) 
                                                    >> 2U));
    } else {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & ((0x31U 
                                                     == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                                                    & ((IData)(vlSelfRef.audio_dmas) 
                                                       >> 3U)));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & ((0x31U 
                                                     == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                                                    & ((IData)(vlSelfRef.audio_dmal) 
                                                       >> 3U)));
    }
    vlSelfRef.strhor_denise = ((0xbU == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                               & ((8U < (IData)(vlSelfRef.agnus__DOT__vpos)) 
                                  | (IData)(vlSelfRef.ecs)));
    __Vtableidx2 = vlSelfRef.agnus__DOT__hpos;
    vlSelfRef.agnus__DOT__aud1__DOT__channel = Vagnus__ConstPool__TABLE_h4848e397_0
        [__Vtableidx2];
    vlSelfRef.strhor_paula = (0xdU == (IData)(vlSelfRef.agnus__DOT__hpos));
    __Vtableidx1 = (((IData)(vlSelfRef.floppy_speed) 
                     << 9U) | (IData)(vlSelfRef.agnus__DOT__hpos));
    agnus__DOT__dsk1__DOT__dmaslot = Vagnus__ConstPool__TABLE_h3d6e5953_0
        [__Vtableidx1];
    vlSelfRef.agnus__DOT__bc1__DOT__last_line = ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame)
                                                  ? (IData)(vlSelfRef.agnus__DOT__bc1__DOT__extra_line)
                                                  : (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vpos_equ_vtotal));
    vlSelfRef.agnus__DOT__spr1__DOT__dmastate_in = 
        ((~ ((IData)(vlSelfRef.agnus__DOT__vbl) | (
                                                   ((((IData)(vlSelfRef.ecs) 
                                                      & (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_1)) 
                                                     << 9U) 
                                                    | (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_4)) 
                                                   == 
                                                   (0x3ffU 
                                                    & (IData)(vlSelfRef.agnus__DOT__vpos))))) 
         & ((((((IData)(vlSelfRef.ecs) & (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_0)) 
               << 9U) | (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_3)) 
             == (0x3ffU & (IData)(vlSelfRef.agnus__DOT__vpos))) 
            | vlSelfRef.agnus__DOT__spr1__DOT__dmastate_mem
            [vlSelfRef.agnus__DOT__spr1__DOT__sprsel]));
    agnus__DOT__reg_address_spr = (((IData)(vlSelfRef.agnus__DOT__spr1__DOT__enable) 
                                    & (1U == (3U & (IData)(vlSelfRef.agnus__DOT__hpos))))
                                    ? (((IData)(vlSelfRef.agnus__DOT__vblend) 
                                        | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprvstop) 
                                           & (~ (IData)(vlSelfRef.agnus__DOT__vbl))))
                                        ? ((4U & (IData)(vlSelfRef.agnus__DOT__hpos))
                                            ? (0xa0U 
                                               | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                  << 2U))
                                            : (0xa1U 
                                               | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                  << 2U)))
                                        : ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprdmastate)
                                            ? ((4U 
                                                & (IData)(vlSelfRef.agnus__DOT__hpos))
                                                ? (0xa2U 
                                                   | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                      << 2U))
                                                : (0xa3U 
                                                   | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                      << 2U)))
                                            : 0xffU))
                                    : 0xffU);
    agnus__DOT__req_spr = (((IData)(vlSelfRef.agnus__DOT__spr1__DOT__enable) 
                            & (1U == (3U & (IData)(vlSelfRef.agnus__DOT__hpos)))) 
                           && (((IData)(vlSelfRef.agnus__DOT__vblend) 
                                | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprvstop) 
                                   & (~ (IData)(vlSelfRef.agnus__DOT__vbl)))) 
                               || (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprdmastate)));
    vlSelfRef.agnus__DOT__aud1__DOT__audptout = vlSelfRef.agnus__DOT__aud1__DOT__audpt
        [vlSelfRef.agnus__DOT__aud1__DOT__channel];
    __Vtableidx3 = vlSelfRef.agnus__DOT__aud1__DOT__channel;
    agnus__DOT__reg_address_aud = Vagnus__ConstPool__TABLE_h2dee94ce_0
        [__Vtableidx3];
    agnus__DOT__dma_ref = ((9U == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                           | ((IData)(vlSelfRef.strhor_paula) 
                              | ((0x11U == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                                 | (0x15U == (IData)(vlSelfRef.agnus__DOT__hpos)))));
    vlSelfRef.agnus__DOT__dma_dsk = ((IData)(vlSelfRef.disk_dmal) 
                                     & ((IData)(agnus__DOT__dsk1__DOT__dmaslot) 
                                        & (IData)(vlSelfRef.cck)));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16 
        = ((1U & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstrt) 
                  ^ (IData)(vlSelfRef.agnus__DOT__vpos)))
            ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod)
            : (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod));
    vlSelfRef.sof = ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__vpos_inc) 
                     & (IData)(vlSelfRef.agnus__DOT__bc1__DOT__last_line));
    agnus__DOT__dma_spr = ((IData)(agnus__DOT__req_spr) 
                           & (IData)((0x220U == (0x220U 
                                                 & (IData)(vlSelfRef.agnus__DOT__dmacon)))));
    vlSelfRef.refresh = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                         && (IData)(agnus__DOT__dma_ref));
    if ((0x4000U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))) {
        vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan 
            = agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16;
        vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan 
            = agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    } else {
        vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan 
            = vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod;
        vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan 
            = vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod;
    }
    vlSelfRef.agnus__DOT__ack_spr = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                                     && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                                         && ((1U & 
                                              (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                             && ((1U 
                                                  & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                                 && (IData)(agnus__DOT__dma_spr)))));
    agnus__DOT__ack_cop = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                           && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                               && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                   && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                       && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                           && (IData)(agnus__DOT__dma_cop))))));
    vlSelfRef.agnus__DOT__bl1__DOT__enable = ((IData)(vlSelfRef.cck) 
                                              & ((~ 
                                                  ((IData)(vlSelfRef.agnus__DOT__dma_dsk) 
                                                   | ((IData)(agnus__DOT__dma_ref) 
                                                      | ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal) 
                                                         | ((IData)(agnus__DOT__dma_spr) 
                                                            | ((IData)(vlSelfRef.agnus__DOT__dma_bpl) 
                                                               | ((IData)(agnus__DOT__dma_cop) 
                                                                  | (3U 
                                                                     == (IData)(vlSelfRef.agnus__DOT__bls_cnt))))))))) 
                                                 & (IData)(vlSelfRef.agnus__DOT__blten)));
    vlSelfRef.agnus__DOT__bpd1__DOT__newpt = (0xfffffU 
                                              & (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                                   ? 
                                                  ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_9) 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                                      >> 1U))
                                                   : 
                                                  ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                    ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_9)
                                                    : (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfend)))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(agnus__DOT__bpd1__DOT__plane))
                                                   ? 
                                                  (agnus__DOT__address_bpl 
                                                   + 
                                                   (((0xf8000U 
                                                      & ((- (IData)(
                                                                    (1U 
                                                                     & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan) 
                                                                        >> 0xeU)))) 
                                                         << 0xfU)) 
                                                     | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan)) 
                                                    + vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17))
                                                   : 
                                                  (agnus__DOT__address_bpl 
                                                   + 
                                                   (((0xf8000U 
                                                      & ((- (IData)(
                                                                    (1U 
                                                                     & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan) 
                                                                        >> 0xeU)))) 
                                                         << 0xfU)) 
                                                     | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan)) 
                                                    + vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17)))
                                                  : 
                                                 (agnus__DOT__address_bpl 
                                                  + vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17)));
    vlSelfRef.agnus__DOT__cp1__DOT__dma_ack = ((IData)(agnus__DOT__ack_cop) 
                                               & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__enable));
    vlSelfRef.agnus__DOT__cp1__DOT__copper_next = (
                                                   (4U 
                                                    & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                    ? 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                     ? 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                      ? 
                                                     ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                       ? 7U
                                                       : 6U)
                                                      : 
                                                     ((1U 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                                       ? 
                                                      ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                        ? 6U
                                                        : 4U)
                                                       : 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__beam_match_wait)
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                         ? 6U
                                                         : 4U)
                                                        : 6U)))
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                      ? 
                                                     (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                       & (IData)(agnus__DOT__cp1__DOT__illegalreg))
                                                       ? 5U
                                                       : 
                                                      (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                        & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag))
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                         ? 4U
                                                         : 5U)
                                                        : 
                                                       ((1U 
                                                         & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                                         ? 
                                                        ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                          ? 7U
                                                          : 5U)
                                                         : 
                                                        ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                          ? 4U
                                                          : 5U))))
                                                      : 
                                                     ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                       ? 5U
                                                       : 4U)))
                                                    : 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                     ? 4U
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                      ? 4U
                                                      : 
                                                     ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                       ? 4U
                                                       : 0U))));
    vlSelfRef.agnus__DOT__bl1__DOT__decbsh = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
                                              & (0x18U 
                                                 == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)));
    vlSelfRef.int3 = ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_9) 
                        & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt))) 
                       | (0x18U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                      & ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__height_cnt)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)));
    vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_dec = 
        ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__next_word));
    agnus__DOT__dma_blt = ((((0x10U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                              ? ((1U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                        >> 3U)) && 
                                 ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                            >> 2U))) 
                                  && ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                       ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                          && (1U & 
                                              ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 9U)))
                                       : ((1U & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                          && (1U & 
                                              (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                >> 9U) 
                                               & ((IData)(
                                                          (0x12U 
                                                           != 
                                                           (0x12U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                                                  | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_pixel))))))))
                              : ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                  ? ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                               >> 2U))) 
                                     && (1U & ((2U 
                                                & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                ? (
                                                   (1U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                                   || (1U 
                                                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                          >> 9U)))
                                                : (
                                                   (1U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                    ? 
                                                   ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                    >> 0xbU)
                                                    : 
                                                   ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__used) 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__pipeline_full))))))
                                  : ((1U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                            >> 2U)) 
                                     && ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                   >> 1U))) 
                                         && (1U & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))))))) 
                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)) 
                           & (IData)(vlSelfRef.agnus__DOT__blten));
    vlSelfRef.agnus__DOT__cp1__DOT__skip = ((1U & ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                   >> 2U)) 
                                            && ((2U 
                                                 & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                 ? 
                                                ((1U 
                                                  & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))) 
                                                 && ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                     && (IData)(vlSelfRef.agnus__DOT__cp1__DOT__beam_match_skip)))
                                                 : 
                                                ((1U 
                                                  & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                  ? 
                                                 ((1U 
                                                   & (~ 
                                                      ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                       & (IData)(agnus__DOT__cp1__DOT__illegalreg)))) 
                                                  && (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag)) 
                                                      && (1U 
                                                          & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)))))
                                                  : (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag))));
    vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_rld = 
        (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_dec) 
          & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt))) 
         | ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)));
    vlSelfRef.dbr = ((IData)(vlSelfRef.agnus__DOT__dma_dsk) 
                     || ((IData)(agnus__DOT__dma_ref) 
                         || ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal) 
                             || ((IData)(vlSelfRef.agnus__DOT__dma_bpl) 
                                 || ((IData)(agnus__DOT__dma_spr) 
                                     || ((IData)(agnus__DOT__dma_cop) 
                                         || ((IData)(agnus__DOT__dma_blt) 
                                             & (3U 
                                                != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))))))));
    vlSelfRef.cpu_custom = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                            && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                                && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                    && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                        && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                            && ((1U 
                                                 & (~ (IData)(agnus__DOT__dma_cop))) 
                                                && (1U 
                                                    & (~ 
                                                       ((IData)(agnus__DOT__dma_blt) 
                                                        & (3U 
                                                           != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))))))))));
    agnus__DOT__ack_blt = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                           && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                               && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                   && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                       && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                           && ((1U 
                                                & (~ (IData)(agnus__DOT__dma_cop))) 
                                               && ((IData)(agnus__DOT__dma_blt) 
                                                   & (3U 
                                                      != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))))))));
    if ((0x10U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__blt_next = 
            ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
              ? ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 0U : ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                               ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? 0x1aU : 0x1bU)
                               : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? 0x18U : 0x1aU))
                           : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                               ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? 0x1bU : 0x19U)
                               : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__height_cnt))
                                       ? 0U : 0x19U)
                                   : 0x18U)))) : 0U);
        vlSelfRef.agnus__DOT__bl1__DOT__enaptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                                            : 
                                                           ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel)) 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)))
                                                           : (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable))));
    } else if ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__blt_next = 
            ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
              ? ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 0U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? 0U : ((IData)(vlSelfRef.cck)
                                    ? 4U : 0xcU))) : 
             ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
               ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                   ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                       ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_11)
                       : 0xbU) : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__used)
                                       ? 8U : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_12)
                                                ? 0U
                                                : 9U))
                                   : 0xaU)) : ((1U 
                                                & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                                    ? 
                                                   ((0x400U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))
                                                     ? 0xbU
                                                     : (IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_11))
                                                    : 9U)
                                                : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                                    ? 
                                                   ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_12)
                                                     ? 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__used)
                                                      ? 0xcU
                                                      : 0U)
                                                     : 9U)
                                                    : 8U))));
        vlSelfRef.agnus__DOT__bl1__DOT__enaptr = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && (IData)(agnus__DOT__ack_blt));
    } else {
        vlSelfRef.agnus__DOT__bl1__DOT__blt_next = 
            ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
              ? ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 0U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? 0U : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                    ? 0U : 4U))) : 
             ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
               ? 0U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                        ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                            ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))
                                ? 0x19U : 9U) : 1U)
                        : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                            ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__start)
                                ? 1U : 0U) : 0U))));
        vlSelfRef.agnus__DOT__bl1__DOT__enaptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && (IData)(agnus__DOT__ack_blt))));
    }
    agnus__DOT__we_blt = ((IData)(agnus__DOT__ack_blt) 
                          & (3U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__chsel)));
    if (vlSelfRef.agnus__DOT__dma_dsk) {
        vlSelfRef.agnus__DOT__dsk1__DOT__address_outnew 
            = (0xfffffU & ((IData)(1U) + vlSelfRef.agnus__DOT__address_dsk));
        vlSelfRef.agnus__DOT__reg_address = (0xffU 
                                             & ((IData)(vlSelfRef.disk_dmas)
                                                 ? 0x13U
                                                 : 4U));
    } else {
        vlSelfRef.agnus__DOT__dsk1__DOT__address_outnew 
            = (0xfffffU & vlSelfRef.__VdfgRegularize_h3c5bdd75_0_2);
        vlSelfRef.agnus__DOT__reg_address = (0xffU 
                                             & ((IData)(agnus__DOT__dma_ref)
                                                 ? 0xffU
                                                 : 
                                                ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal)
                                                  ? (IData)(agnus__DOT__reg_address_aud)
                                                  : 
                                                 ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                   ? (IData)(agnus__DOT__reg_address_bpl)
                                                   : 
                                                  ((IData)(agnus__DOT__dma_spr)
                                                    ? (IData)(agnus__DOT__reg_address_spr)
                                                    : 
                                                   ((IData)(agnus__DOT__dma_cop)
                                                     ? 
                                                    (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__enable) 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__selins))
                                                      ? 0x46U
                                                      : 
                                                     (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__enable) 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__selreg))
                                                       ? 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2) 
                                                       >> 1U)
                                                       : 0xffU))
                                                     : 
                                                    (((IData)(agnus__DOT__dma_blt) 
                                                      & (3U 
                                                         != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))
                                                      ? (IData)(vlSelfRef.agnus__DOT__reg_address_blt)
                                                      : 
                                                     (((IData)(vlSelfRef.aen) 
                                                       & ((IData)(vlSelfRef.rd) 
                                                          | ((IData)(vlSelfRef.hwr) 
                                                             | (IData)(vlSelfRef.lwr))))
                                                       ? (IData)(vlSelfRef.address_in)
                                                       : 0xffU))))))));
    }
    vlSelfRef.reg_address_out = vlSelfRef.agnus__DOT__reg_address;
    vlSelfRef.agnus__DOT__aud1__DOT__audlcena = (1U 
                                                 & (IData)(
                                                           ((0x40U 
                                                             == 
                                                             (0xc0U 
                                                              & (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                                                            & ((~ (IData)(
                                                                          (0U 
                                                                           != 
                                                                           (6U 
                                                                            & (IData)(vlSelfRef.agnus__DOT__reg_address))))) 
                                                               & (((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                                   >> 5U) 
                                                                  ^ 
                                                                  ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                                   >> 4U))))));
    vlSelfRef.agnus__DOT__aud1__DOT__audlcsel = ((2U 
                                                  & ((~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                       >> 4U)) 
                                                     << 1U)) 
                                                 | (1U 
                                                    & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                       >> 3U)));
    vlSelfRef.agnus__DOT__bpd1__DOT__bplptr_sel = (7U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                       ? (IData)(agnus__DOT__bpd1__DOT__plane)
                                                       : 
                                                      ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                       >> 1U)));
    if (vlSelfRef.agnus__DOT__ack_spr) {
        vlSelfRef.agnus__DOT__spr1__DOT__ptsel = (7U 
                                                  & (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite));
        vlSelfRef.agnus__DOT__spr1__DOT__pcsel = (7U 
                                                  & (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite));
    } else {
        vlSelfRef.agnus__DOT__spr1__DOT__ptsel = (7U 
                                                  & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                     >> 1U));
        vlSelfRef.agnus__DOT__spr1__DOT__pcsel = (7U 
                                                  & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                     >> 2U));
    }
    vlSelfRef.data_out = ((((2U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                            | (0x15U == (IData)(vlSelfRef.agnus__DOT__reg_address)))
                            ? (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame) 
                                << 0xfU) | (((IData)(vlSelfRef.ecs) 
                                             << 0xdU) 
                                            | (((IData)(vlSelfRef.ntsc) 
                                                << 0xcU) 
                                               | ((0x300U 
                                                   & ((- (IData)((IData)(vlSelfRef.aga))) 
                                                      << 8U)) 
                                                  | (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_line) 
                                                      << 7U) 
                                                     | (7U 
                                                        & ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                                           >> 8U)))))))
                            : (((3U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                                | (0x16U == (IData)(vlSelfRef.agnus__DOT__reg_address)))
                                ? ((0xff00U & ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                               << 8U)) 
                                   | (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r))
                                : 0U)) | (((1U == (IData)(vlSelfRef.agnus__DOT__reg_address))
                                            ? (((IData)(vlSelfRef.agnus__DOT__blit_busy) 
                                                << 0xeU) 
                                               | (((IData)(vlSelfRef.agnus__DOT__blit_zero) 
                                                   << 0xdU) 
                                                  | (IData)(vlSelfRef.agnus__DOT__dmacon)))
                                            : 0U) | 
                                          ((IData)(agnus__DOT__we_blt)
                                            ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltdhold)
                                            : 0U)));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel 
        = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enaptr)
            ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__ptrsel)
            : ((2U & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                      >> 2U)) | (1U & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                       >> 1U))));
    agnus__DOT__address_blt = ((vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth
                                [vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel] 
                                << 0xfU) | vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl
                               [vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel]);
    if (vlSelfRef.agnus__DOT__dma_dsk) {
        vlSelfRef.dbwe = (1U & (~ (IData)(vlSelfRef.disk_dmas)));
        vlSelfRef.address_out = vlSelfRef.agnus__DOT__address_dsk;
    } else {
        vlSelfRef.dbwe = (1U & ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                                && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                    && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                        && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                            && ((1U 
                                                 & (~ (IData)(agnus__DOT__dma_cop))) 
                                                && (((IData)(agnus__DOT__dma_blt) 
                                                     & (3U 
                                                        != (IData)(vlSelfRef.agnus__DOT__bls_cnt))) 
                                                    && (IData)(agnus__DOT__we_blt))))))));
        vlSelfRef.address_out = ((IData)(agnus__DOT__dma_ref)
                                  ? 0U : ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal)
                                           ? vlSelfRef.agnus__DOT__aud1__DOT__audpt
                                          [vlSelfRef.agnus__DOT__aud1__DOT__channel]
                                           : ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                               ? agnus__DOT__address_bpl
                                               : ((IData)(agnus__DOT__dma_spr)
                                                   ? vlSelfRef.agnus__DOT__address_spr
                                                   : 
                                                  ((IData)(agnus__DOT__dma_cop)
                                                    ? vlSelfRef.agnus__DOT__address_cop
                                                    : 
                                                   (((IData)(agnus__DOT__dma_blt) 
                                                     & (3U 
                                                        != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))
                                                     ? agnus__DOT__address_blt
                                                     : 0U))))));
    }
    agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr 
        = (0xfffffU & (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__decptr)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__incptr))
                        ? ((IData)(1U) + agnus__DOT__address_blt)
                        : (agnus__DOT__address_blt 
                           - ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__incptr)) 
                              & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__decptr)))));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__newptr 
        = (0xfffffU & (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__submod)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__addmod))
                        ? (agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr 
                           + vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT____VdfgRegularize_h62043dcc_0_0)
                        : (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__addmod)) 
                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__submod))
                            ? (agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr 
                               - vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT____VdfgRegularize_h62043dcc_0_0)
                            : agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr)));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_in 
        = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enaptr)
            ? vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__newptr
            : vlSelfRef.__VdfgRegularize_h3c5bdd75_0_2);
}

void Vagnus___024root___eval_triggers__act(Vagnus___024root* vlSelf);

bool Vagnus___024root___eval_phase__act(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_phase__act\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<2> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus___024root___eval_phase__nba(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_phase__nba\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus___024root___dump_triggers__ico(Vagnus___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus___024root___dump_triggers__nba(Vagnus___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus___024root___dump_triggers__act(Vagnus___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus___024root___eval(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus.v", 54, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vagnus___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus.v", 54, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../../src/minimig-aga/agnus.v", 54, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus___024root___eval_debug_assertions(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_debug_assertions\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.clk7_en & 0xfeU)))) {
        Verilated::overWidthError("clk7_en");}
    if (VL_UNLIKELY(((vlSelfRef.cck & 0xfeU)))) {
        Verilated::overWidthError("cck");}
    if (VL_UNLIKELY(((vlSelfRef.reset & 0xfeU)))) {
        Verilated::overWidthError("reset");}
    if (VL_UNLIKELY(((vlSelfRef.aen & 0xfeU)))) {
        Verilated::overWidthError("aen");}
    if (VL_UNLIKELY(((vlSelfRef.rd & 0xfeU)))) {
        Verilated::overWidthError("rd");}
    if (VL_UNLIKELY(((vlSelfRef.hwr & 0xfeU)))) {
        Verilated::overWidthError("hwr");}
    if (VL_UNLIKELY(((vlSelfRef.lwr & 0xfeU)))) {
        Verilated::overWidthError("lwr");}
    if (VL_UNLIKELY(((vlSelfRef.audio_dmal & 0xf0U)))) {
        Verilated::overWidthError("audio_dmal");}
    if (VL_UNLIKELY(((vlSelfRef.audio_dmas & 0xf0U)))) {
        Verilated::overWidthError("audio_dmas");}
    if (VL_UNLIKELY(((vlSelfRef.disk_dmal & 0xfeU)))) {
        Verilated::overWidthError("disk_dmal");}
    if (VL_UNLIKELY(((vlSelfRef.disk_dmas & 0xfeU)))) {
        Verilated::overWidthError("disk_dmas");}
    if (VL_UNLIKELY(((vlSelfRef.bls & 0xfeU)))) {
        Verilated::overWidthError("bls");}
    if (VL_UNLIKELY(((vlSelfRef.ntsc & 0xfeU)))) {
        Verilated::overWidthError("ntsc");}
    if (VL_UNLIKELY(((vlSelfRef.a1k & 0xfeU)))) {
        Verilated::overWidthError("a1k");}
    if (VL_UNLIKELY(((vlSelfRef.ecs & 0xfeU)))) {
        Verilated::overWidthError("ecs");}
    if (VL_UNLIKELY(((vlSelfRef.aga & 0xfeU)))) {
        Verilated::overWidthError("aga");}
    if (VL_UNLIKELY(((vlSelfRef.floppy_speed & 0xfeU)))) {
        Verilated::overWidthError("floppy_speed");}
}
#endif  // VL_DEBUG
