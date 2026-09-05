// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_copper.h for the primary calling header

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper___024root.h"

void Vagnus_copper___024root___ico_sequent__TOP__0(Vagnus_copper___024root* vlSelf);

void Vagnus_copper___024root___eval_ico(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_ico\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus_copper___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_copper___024root___ico_sequent__TOP__0(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___ico_sequent__TOP__0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus_copper__DOT__illegalreg;
    agnus_copper__DOT__illegalreg = 0;
    CData/*0:0*/ agnus_copper__DOT__dma_req;
    agnus_copper__DOT__dma_req = 0;
    CData/*0:0*/ agnus_copper__DOT____VdfgRegularize_he23179ad_0_0;
    agnus_copper__DOT____VdfgRegularize_he23179ad_0_0 = 0;
    // Body
    vlSelfRef.agnus_copper__DOT__beam_match_skip = 
        ((IData)(vlSelfRef.agnus_copper__DOT__beam_match) 
         & ((~ (IData)(vlSelfRef.blit_busy)) | ((IData)(vlSelfRef.agnus_copper__DOT__ir2) 
                                                >> 0xfU)));
    agnus_copper__DOT____VdfgRegularize_he23179ad_0_0 
        = ((IData)(vlSelfRef.agnus_copper__DOT__bus_ena) 
           & (IData)(vlSelfRef.hpos));
    agnus_copper__DOT__illegalreg = (((~ (IData)(vlSelfRef.agnus_copper__DOT__cdang)) 
                                      & (0U == (0x180U 
                                                & (IData)(vlSelfRef.agnus_copper__DOT__ir2)))) 
                                     | ((~ (IData)(vlSelfRef.ecs)) 
                                        & (0U == (0x1c0U 
                                                  & (IData)(vlSelfRef.agnus_copper__DOT__ir2)))));
    vlSelfRef.agnus_copper__DOT__enable = ((~ (IData)(vlSelfRef.agnus_copper__DOT__bus_blk)) 
                                           & (IData)(agnus_copper__DOT____VdfgRegularize_he23179ad_0_0));
    vlSelfRef.agnus_copper__DOT__selreg = ((1U & ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                  >> 2U)) 
                                           && ((1U 
                                                & (~ 
                                                   ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                    >> 1U))) 
                                               && ((1U 
                                                    & (IData)(vlSelfRef.agnus_copper__DOT__copper_state)) 
                                                   && ((1U 
                                                        & (~ 
                                                           ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                            & (IData)(agnus_copper__DOT__illegalreg)))) 
                                                       && ((1U 
                                                            & (~ 
                                                               ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                                & (IData)(vlSelfRef.agnus_copper__DOT__skip_flag)))) 
                                                           && (1U 
                                                               & (~ (IData)(vlSelfRef.agnus_copper__DOT__ir2))))))));
    vlSelfRef.agnus_copper__DOT__selins = ((1U & ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                  >> 2U)) 
                                           && ((1U 
                                                & (~ 
                                                   ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                    >> 1U))) 
                                               && ((1U 
                                                    & (~ (IData)(vlSelfRef.agnus_copper__DOT__copper_state))) 
                                                   || ((1U 
                                                        & (~ 
                                                           ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                            & (IData)(agnus_copper__DOT__illegalreg)))) 
                                                       && (((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                            & (IData)(vlSelfRef.agnus_copper__DOT__skip_flag)) 
                                                           || (1U 
                                                               & (IData)(vlSelfRef.agnus_copper__DOT__ir2)))))));
    vlSelfRef.agnus_copper__DOT__dma_ack = ((IData)(vlSelfRef.ackdma) 
                                            & (IData)(vlSelfRef.agnus_copper__DOT__enable));
    if ((4U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))) {
        agnus_copper__DOT__dma_req = ((1U & (~ ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                >> 1U))) 
                                      && ((1U & (~ (IData)(vlSelfRef.agnus_copper__DOT__copper_state))) 
                                          || (1U & 
                                              (~ ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                  & (IData)(agnus_copper__DOT__illegalreg))))));
        vlSelfRef.agnus_copper__DOT__copper_next = 
            ((2U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
              ? ((1U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                  ? ((IData)(vlSelfRef.enadma) ? 6U
                      : 7U) : ((1U & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                ? ((IData)(vlSelfRef.enadma)
                                    ? 4U : 6U) : ((IData)(vlSelfRef.agnus_copper__DOT__beam_match_wait)
                                                   ? 
                                                  ((IData)(vlSelfRef.enadma)
                                                    ? 4U
                                                    : 6U)
                                                   : 6U)))
              : ((1U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                  ? (((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                      & (IData)(agnus_copper__DOT__illegalreg))
                      ? 5U : (((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                               & (IData)(vlSelfRef.agnus_copper__DOT__skip_flag))
                               ? ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                                   ? 4U : 5U) : ((1U 
                                                  & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                  ? 
                                                 ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                                                   ? 7U
                                                   : 5U)
                                                  : 
                                                 ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                                                   ? 4U
                                                   : 5U))))
                  : ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                      ? 5U : 4U)));
    } else {
        agnus_copper__DOT__dma_req = ((1U & (~ ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                >> 1U))) 
                                      && (1U & (~ (IData)(vlSelfRef.agnus_copper__DOT__copper_state))));
        vlSelfRef.agnus_copper__DOT__copper_next = 
            ((2U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
              ? 4U : ((1U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                       ? 4U : ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                                ? 4U : 0U)));
    }
    vlSelfRef.reqdma = ((IData)(agnus_copper__DOT__dma_req) 
                        & (IData)(agnus_copper__DOT____VdfgRegularize_he23179ad_0_0));
    vlSelfRef.reg_address_out = (((IData)(vlSelfRef.agnus_copper__DOT__enable) 
                                  & (IData)(vlSelfRef.agnus_copper__DOT__selins))
                                  ? 0x46U : (((IData)(vlSelfRef.agnus_copper__DOT__enable) 
                                              & (IData)(vlSelfRef.agnus_copper__DOT__selreg))
                                              ? (0xffU 
                                                 & ((IData)(vlSelfRef.agnus_copper__DOT__ir2) 
                                                    >> 1U))
                                              : 0xffU));
    vlSelfRef.agnus_copper__DOT__skip = ((1U & ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                >> 2U)) 
                                         && ((2U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                                              ? ((1U 
                                                  & (~ (IData)(vlSelfRef.agnus_copper__DOT__copper_state))) 
                                                 && ((1U 
                                                      & (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                     && (IData)(vlSelfRef.agnus_copper__DOT__beam_match_skip)))
                                              : ((1U 
                                                  & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                                                  ? 
                                                 ((1U 
                                                   & (~ 
                                                      ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                       & (IData)(agnus_copper__DOT__illegalreg)))) 
                                                  && (((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                       & (IData)(vlSelfRef.agnus_copper__DOT__skip_flag)) 
                                                      && (1U 
                                                          & (~ (IData)(vlSelfRef.agnus_copper__DOT__dma_ack)))))
                                                  : (IData)(vlSelfRef.agnus_copper__DOT__skip_flag))));
}

void Vagnus_copper___024root___eval_triggers__ico(Vagnus_copper___024root* vlSelf);

bool Vagnus_copper___024root___eval_phase__ico(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_phase__ico\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VicoExecute;
    // Body
    Vagnus_copper___024root___eval_triggers__ico(vlSelf);
    __VicoExecute = vlSelfRef.__VicoTriggered.any();
    if (__VicoExecute) {
        Vagnus_copper___024root___eval_ico(vlSelf);
    }
    return (__VicoExecute);
}

void Vagnus_copper___024root___eval_act(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_act\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

void Vagnus_copper___024root___nba_sequent__TOP__0(Vagnus_copper___024root* vlSelf);

void Vagnus_copper___024root___eval_nba(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_nba\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus_copper___024root___nba_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_copper___024root___nba_sequent__TOP__0(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___nba_sequent__TOP__0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus_copper__DOT__illegalreg;
    agnus_copper__DOT__illegalreg = 0;
    CData/*0:0*/ agnus_copper__DOT__dma_req;
    agnus_copper__DOT__dma_req = 0;
    CData/*0:0*/ agnus_copper__DOT____VdfgRegularize_he23179ad_0_0;
    agnus_copper__DOT____VdfgRegularize_he23179ad_0_0 = 0;
    IData/*19:0*/ __Vdly__address_out;
    __Vdly__address_out = 0;
    // Body
    __Vdly__address_out = vlSelfRef.address_out;
    if (vlSelfRef.clk7_en) {
        if ((((IData)(vlSelfRef.agnus_copper__DOT__dma_ack) 
              & (IData)(vlSelfRef.agnus_copper__DOT__strobe1)) 
             & (0U == (IData)(vlSelfRef.agnus_copper__DOT__copper_state)))) {
            __Vdly__address_out = (((IData)(vlSelfRef.agnus_copper__DOT__cop1lch) 
                                    << 0xfU) | (IData)(vlSelfRef.agnus_copper__DOT__cop1lcl));
        } else if ((((IData)(vlSelfRef.agnus_copper__DOT__dma_ack) 
                     & (IData)(vlSelfRef.agnus_copper__DOT__strobe2)) 
                    & (0U == (IData)(vlSelfRef.agnus_copper__DOT__copper_state)))) {
            __Vdly__address_out = (((IData)(vlSelfRef.agnus_copper__DOT__cop2lch) 
                                    << 0xfU) | (IData)(vlSelfRef.agnus_copper__DOT__cop2lcl));
        } else if (((IData)(vlSelfRef.agnus_copper__DOT__dma_ack) 
                    & ((IData)(vlSelfRef.agnus_copper__DOT__selins) 
                       | (IData)(vlSelfRef.agnus_copper__DOT__selreg)))) {
            __Vdly__address_out = (0xfffffU & ((IData)(1U) 
                                               + vlSelfRef.address_out));
        }
        if (vlSelfRef.agnus_copper__DOT__enable) {
            vlSelfRef.agnus_copper__DOT__skip_flag 
                = vlSelfRef.agnus_copper__DOT__skip;
        }
        if (vlSelfRef.reset) {
            vlSelfRef.agnus_copper__DOT__cdang = 0U;
            vlSelfRef.agnus_copper__DOT__cop1lch = 0U;
            vlSelfRef.agnus_copper__DOT__cop1lcl = 0U;
            vlSelfRef.agnus_copper__DOT__cop2lch = 0U;
            vlSelfRef.agnus_copper__DOT__cop2lcl = 0U;
        } else {
            if ((0x17U == (IData)(vlSelfRef.reg_address_in))) {
                vlSelfRef.agnus_copper__DOT__cdang 
                    = (1U & ((IData)(vlSelfRef.data_in) 
                             >> 1U));
            }
            if ((0x40U == (IData)(vlSelfRef.reg_address_in))) {
                vlSelfRef.agnus_copper__DOT__cop1lch 
                    = (0x1fU & (IData)(vlSelfRef.data_in));
            }
            if ((0x41U == (IData)(vlSelfRef.reg_address_in))) {
                vlSelfRef.agnus_copper__DOT__cop1lcl 
                    = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                                  >> 1U));
            }
            if ((0x42U == (IData)(vlSelfRef.reg_address_in))) {
                vlSelfRef.agnus_copper__DOT__cop2lch 
                    = (0x1fU & (IData)(vlSelfRef.data_in));
            }
            if ((0x43U == (IData)(vlSelfRef.reg_address_in))) {
                vlSelfRef.agnus_copper__DOT__cop2lcl 
                    = (0x7fffU & ((IData)(vlSelfRef.data_in) 
                                  >> 1U));
            }
        }
        if (((IData)(vlSelfRef.agnus_copper__DOT__copjmp2) 
             & (IData)(vlSelfRef.hpos))) {
            vlSelfRef.agnus_copper__DOT__strobe2 = 1U;
        } else if (((0U == (IData)(vlSelfRef.agnus_copper__DOT__copper_state)) 
                    & (IData)(vlSelfRef.agnus_copper__DOT__dma_ack))) {
            vlSelfRef.agnus_copper__DOT__strobe2 = 0U;
        }
        if (((IData)(vlSelfRef.agnus_copper__DOT__copjmp1) 
             & (IData)(vlSelfRef.hpos))) {
            vlSelfRef.agnus_copper__DOT__strobe1 = 1U;
        } else if (((0U == (IData)(vlSelfRef.agnus_copper__DOT__copper_state)) 
                    & (IData)(vlSelfRef.agnus_copper__DOT__dma_ack))) {
            vlSelfRef.agnus_copper__DOT__strobe1 = 0U;
        }
        if ((1U & (IData)(vlSelfRef.hpos))) {
            vlSelfRef.agnus_copper__DOT__bus_ena = 
                (1U & ((IData)(vlSelfRef.agnus_copper__DOT__bus_blk) 
                       | (~ (IData)(vlSelfRef.agnus_copper__DOT__bus_ena))));
            vlSelfRef.agnus_copper__DOT__beam_match_wait 
                = vlSelfRef.agnus_copper__DOT__beam_match_skip;
            vlSelfRef.agnus_copper__DOT__beam_match 
                = ((((IData)(vlSelfRef.vpos) << 7U) 
                    | (0x7fU & ((IData)(vlSelfRef.hpos) 
                                >> 2U))) >= (((((0x4000U 
                                                 & (IData)(vlSelfRef.agnus_copper__DOT__ir1)) 
                                                | (0x2000U 
                                                   & (((0x4000U 
                                                        & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                        >> 0xdU)
                                                        : 
                                                       ((IData)(vlSelfRef.vpos) 
                                                        >> 6U)) 
                                                      << 0xdU))) 
                                               | ((0x1000U 
                                                   & (((0x2000U 
                                                        & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                        >> 0xcU)
                                                        : 
                                                       ((IData)(vlSelfRef.vpos) 
                                                        >> 5U)) 
                                                      << 0xcU)) 
                                                  | (0x800U 
                                                     & (((0x1000U 
                                                          & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                          ? 
                                                         ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                          >> 0xbU)
                                                          : 
                                                         ((IData)(vlSelfRef.vpos) 
                                                          >> 4U)) 
                                                        << 0xbU)))) 
                                              | (((0x400U 
                                                   & (((0x800U 
                                                        & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                        >> 0xaU)
                                                        : 
                                                       ((IData)(vlSelfRef.vpos) 
                                                        >> 3U)) 
                                                      << 0xaU)) 
                                                  | (0x200U 
                                                     & (((0x400U 
                                                          & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                          ? 
                                                         ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                          >> 9U)
                                                          : 
                                                         ((IData)(vlSelfRef.vpos) 
                                                          >> 2U)) 
                                                        << 9U))) 
                                                 | ((0x100U 
                                                     & (((0x200U 
                                                          & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                          ? 
                                                         ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                          >> 8U)
                                                          : 
                                                         ((IData)(vlSelfRef.vpos) 
                                                          >> 1U)) 
                                                        << 8U)) 
                                                    | (0x80U 
                                                       & (((0x100U 
                                                            & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                            >> 7U)
                                                            : (IData)(vlSelfRef.vpos)) 
                                                          << 7U))))) 
                                             | ((((0x40U 
                                                   & (((0x80U 
                                                        & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                        >> 6U)
                                                        : 
                                                       ((IData)(vlSelfRef.hpos) 
                                                        >> 8U)) 
                                                      << 6U)) 
                                                  | (0x20U 
                                                     & (((0x40U 
                                                          & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                          ? 
                                                         ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                          >> 5U)
                                                          : 
                                                         ((IData)(vlSelfRef.hpos) 
                                                          >> 7U)) 
                                                        << 5U))) 
                                                 | ((0x10U 
                                                     & (((0x20U 
                                                          & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                          ? 
                                                         ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                          >> 4U)
                                                          : 
                                                         ((IData)(vlSelfRef.hpos) 
                                                          >> 6U)) 
                                                        << 4U)) 
                                                    | (8U 
                                                       & (((0x10U 
                                                            & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                            >> 3U)
                                                            : 
                                                           ((IData)(vlSelfRef.hpos) 
                                                            >> 5U)) 
                                                          << 3U)))) 
                                                | ((4U 
                                                    & (((8U 
                                                         & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                         ? 
                                                        ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                         >> 2U)
                                                         : 
                                                        ((IData)(vlSelfRef.hpos) 
                                                         >> 4U)) 
                                                       << 2U)) 
                                                   | ((2U 
                                                       & (((4U 
                                                            & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus_copper__DOT__ir1) 
                                                            >> 1U)
                                                            : 
                                                           ((IData)(vlSelfRef.hpos) 
                                                            >> 3U)) 
                                                          << 1U)) 
                                                      | (1U 
                                                         & ((2U 
                                                             & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                             ? (IData)(vlSelfRef.agnus_copper__DOT__ir1)
                                                             : 
                                                            ((IData)(vlSelfRef.hpos) 
                                                             >> 2U))))))));
        }
        if ((0x46U == (IData)(vlSelfRef.reg_address_in))) {
            vlSelfRef.agnus_copper__DOT__ir1 = (0x7fffU 
                                                & ((IData)(vlSelfRef.agnus_copper__DOT__ir2) 
                                                   >> 1U));
            vlSelfRef.agnus_copper__DOT__ir2 = vlSelfRef.data_in;
        }
        if (((IData)(vlSelfRef.reset) | ((IData)(vlSelfRef.hpos) 
                                         & (IData)(vlSelfRef.agnus_copper__DOT__strobe)))) {
            vlSelfRef.agnus_copper__DOT__copper_state = 0U;
        } else if (vlSelfRef.agnus_copper__DOT__enable) {
            vlSelfRef.agnus_copper__DOT__copper_state 
                = vlSelfRef.agnus_copper__DOT__copper_next;
        }
        if ((1U & (IData)(vlSelfRef.hpos))) {
            vlSelfRef.agnus_copper__DOT__bus_blk = 
                (1U == (0xffU & ((IData)(vlSelfRef.hpos) 
                                 >> 1U)));
            vlSelfRef.agnus_copper__DOT__strobe = ((IData)(vlSelfRef.agnus_copper__DOT__copjmp1) 
                                                   | (IData)(vlSelfRef.agnus_copper__DOT__copjmp2));
        }
        if ((0x45U == (IData)(vlSelfRef.reg_address_in))) {
            vlSelfRef.agnus_copper__DOT__copjmp2 = 1U;
        } else if ((1U & (IData)(vlSelfRef.hpos))) {
            vlSelfRef.agnus_copper__DOT__copjmp2 = 0U;
        }
        if (((0x44U == (IData)(vlSelfRef.reg_address_in)) 
             | (IData)(vlSelfRef.sof))) {
            vlSelfRef.agnus_copper__DOT__copjmp1 = 1U;
        } else if ((1U & (IData)(vlSelfRef.hpos))) {
            vlSelfRef.agnus_copper__DOT__copjmp1 = 0U;
        }
    }
    vlSelfRef.address_out = __Vdly__address_out;
    agnus_copper__DOT____VdfgRegularize_he23179ad_0_0 
        = ((IData)(vlSelfRef.agnus_copper__DOT__bus_ena) 
           & (IData)(vlSelfRef.hpos));
    vlSelfRef.agnus_copper__DOT__enable = ((~ (IData)(vlSelfRef.agnus_copper__DOT__bus_blk)) 
                                           & (IData)(agnus_copper__DOT____VdfgRegularize_he23179ad_0_0));
    vlSelfRef.agnus_copper__DOT__dma_ack = ((IData)(vlSelfRef.ackdma) 
                                            & (IData)(vlSelfRef.agnus_copper__DOT__enable));
    vlSelfRef.agnus_copper__DOT__beam_match_skip = 
        ((IData)(vlSelfRef.agnus_copper__DOT__beam_match) 
         & ((~ (IData)(vlSelfRef.blit_busy)) | ((IData)(vlSelfRef.agnus_copper__DOT__ir2) 
                                                >> 0xfU)));
    agnus_copper__DOT__illegalreg = (((~ (IData)(vlSelfRef.agnus_copper__DOT__cdang)) 
                                      & (0U == (0x180U 
                                                & (IData)(vlSelfRef.agnus_copper__DOT__ir2)))) 
                                     | ((~ (IData)(vlSelfRef.ecs)) 
                                        & (0U == (0x1c0U 
                                                  & (IData)(vlSelfRef.agnus_copper__DOT__ir2)))));
    vlSelfRef.agnus_copper__DOT__skip = ((1U & ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                >> 2U)) 
                                         && ((2U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                                              ? ((1U 
                                                  & (~ (IData)(vlSelfRef.agnus_copper__DOT__copper_state))) 
                                                 && ((1U 
                                                      & (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                     && (IData)(vlSelfRef.agnus_copper__DOT__beam_match_skip)))
                                              : ((1U 
                                                  & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                                                  ? 
                                                 ((1U 
                                                   & (~ 
                                                      ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                       & (IData)(agnus_copper__DOT__illegalreg)))) 
                                                  && (((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                       & (IData)(vlSelfRef.agnus_copper__DOT__skip_flag)) 
                                                      && (1U 
                                                          & (~ (IData)(vlSelfRef.agnus_copper__DOT__dma_ack)))))
                                                  : (IData)(vlSelfRef.agnus_copper__DOT__skip_flag))));
    if ((4U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))) {
        vlSelfRef.agnus_copper__DOT__copper_next = 
            ((2U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
              ? ((1U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                  ? ((IData)(vlSelfRef.enadma) ? 6U
                      : 7U) : ((1U & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                ? ((IData)(vlSelfRef.enadma)
                                    ? 4U : 6U) : ((IData)(vlSelfRef.agnus_copper__DOT__beam_match_wait)
                                                   ? 
                                                  ((IData)(vlSelfRef.enadma)
                                                    ? 4U
                                                    : 6U)
                                                   : 6U)))
              : ((1U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                  ? (((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                      & (IData)(agnus_copper__DOT__illegalreg))
                      ? 5U : (((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                               & (IData)(vlSelfRef.agnus_copper__DOT__skip_flag))
                               ? ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                                   ? 4U : 5U) : ((1U 
                                                  & (IData)(vlSelfRef.agnus_copper__DOT__ir2))
                                                  ? 
                                                 ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                                                   ? 7U
                                                   : 5U)
                                                  : 
                                                 ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                                                   ? 4U
                                                   : 5U))))
                  : ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                      ? 5U : 4U)));
        agnus_copper__DOT__dma_req = ((1U & (~ ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                >> 1U))) 
                                      && ((1U & (~ (IData)(vlSelfRef.agnus_copper__DOT__copper_state))) 
                                          || (1U & 
                                              (~ ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                  & (IData)(agnus_copper__DOT__illegalreg))))));
    } else {
        vlSelfRef.agnus_copper__DOT__copper_next = 
            ((2U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
              ? 4U : ((1U & (IData)(vlSelfRef.agnus_copper__DOT__copper_state))
                       ? 4U : ((IData)(vlSelfRef.agnus_copper__DOT__dma_ack)
                                ? 4U : 0U)));
        agnus_copper__DOT__dma_req = ((1U & (~ ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                >> 1U))) 
                                      && (1U & (~ (IData)(vlSelfRef.agnus_copper__DOT__copper_state))));
    }
    vlSelfRef.agnus_copper__DOT__selreg = ((1U & ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                  >> 2U)) 
                                           && ((1U 
                                                & (~ 
                                                   ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                    >> 1U))) 
                                               && ((1U 
                                                    & (IData)(vlSelfRef.agnus_copper__DOT__copper_state)) 
                                                   && ((1U 
                                                        & (~ 
                                                           ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                            & (IData)(agnus_copper__DOT__illegalreg)))) 
                                                       && ((1U 
                                                            & (~ 
                                                               ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                                & (IData)(vlSelfRef.agnus_copper__DOT__skip_flag)))) 
                                                           && (1U 
                                                               & (~ (IData)(vlSelfRef.agnus_copper__DOT__ir2))))))));
    vlSelfRef.agnus_copper__DOT__selins = ((1U & ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                  >> 2U)) 
                                           && ((1U 
                                                & (~ 
                                                   ((IData)(vlSelfRef.agnus_copper__DOT__copper_state) 
                                                    >> 1U))) 
                                               && ((1U 
                                                    & (~ (IData)(vlSelfRef.agnus_copper__DOT__copper_state))) 
                                                   || ((1U 
                                                        & (~ 
                                                           ((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                            & (IData)(agnus_copper__DOT__illegalreg)))) 
                                                       && (((~ (IData)(vlSelfRef.agnus_copper__DOT__ir2)) 
                                                            & (IData)(vlSelfRef.agnus_copper__DOT__skip_flag)) 
                                                           || (1U 
                                                               & (IData)(vlSelfRef.agnus_copper__DOT__ir2)))))));
    vlSelfRef.reqdma = ((IData)(agnus_copper__DOT__dma_req) 
                        & (IData)(agnus_copper__DOT____VdfgRegularize_he23179ad_0_0));
    vlSelfRef.reg_address_out = (((IData)(vlSelfRef.agnus_copper__DOT__enable) 
                                  & (IData)(vlSelfRef.agnus_copper__DOT__selins))
                                  ? 0x46U : (((IData)(vlSelfRef.agnus_copper__DOT__enable) 
                                              & (IData)(vlSelfRef.agnus_copper__DOT__selreg))
                                              ? (0xffU 
                                                 & ((IData)(vlSelfRef.agnus_copper__DOT__ir2) 
                                                    >> 1U))
                                              : 0xffU));
}

void Vagnus_copper___024root___eval_triggers__act(Vagnus_copper___024root* vlSelf);

bool Vagnus_copper___024root___eval_phase__act(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_phase__act\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    VlTriggerVec<1> __VpreTriggered;
    CData/*0:0*/ __VactExecute;
    // Body
    Vagnus_copper___024root___eval_triggers__act(vlSelf);
    __VactExecute = vlSelfRef.__VactTriggered.any();
    if (__VactExecute) {
        __VpreTriggered.andNot(vlSelfRef.__VactTriggered, vlSelfRef.__VnbaTriggered);
        vlSelfRef.__VnbaTriggered.thisOr(vlSelfRef.__VactTriggered);
        Vagnus_copper___024root___eval_act(vlSelf);
    }
    return (__VactExecute);
}

bool Vagnus_copper___024root___eval_phase__nba(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_phase__nba\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VnbaExecute;
    // Body
    __VnbaExecute = vlSelfRef.__VnbaTriggered.any();
    if (__VnbaExecute) {
        Vagnus_copper___024root___eval_nba(vlSelf);
        vlSelfRef.__VnbaTriggered.clear();
    }
    return (__VnbaExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__ico(Vagnus_copper___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__nba(Vagnus_copper___024root* vlSelf);
#endif  // VL_DEBUG
#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__act(Vagnus_copper___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_copper___024root___eval(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_copper___024root___dump_triggers__ico(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_copper.v", 58, "", "Input combinational region did not converge.");
        }
        __VicoIterCount = ((IData)(1U) + __VicoIterCount);
        __VicoContinue = 0U;
        if (Vagnus_copper___024root___eval_phase__ico(vlSelf)) {
            __VicoContinue = 1U;
        }
        vlSelfRef.__VicoFirstIteration = 0U;
    }
    __VnbaIterCount = 0U;
    __VnbaContinue = 1U;
    while (__VnbaContinue) {
        if (VL_UNLIKELY(((0x64U < __VnbaIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_copper___024root___dump_triggers__nba(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_copper.v", 58, "", "NBA region did not converge.");
        }
        __VnbaIterCount = ((IData)(1U) + __VnbaIterCount);
        __VnbaContinue = 0U;
        vlSelfRef.__VactIterCount = 0U;
        vlSelfRef.__VactContinue = 1U;
        while (vlSelfRef.__VactContinue) {
            if (VL_UNLIKELY(((0x64U < vlSelfRef.__VactIterCount)))) {
#ifdef VL_DEBUG
                Vagnus_copper___024root___dump_triggers__act(vlSelf);
#endif
                VL_FATAL_MT("../../../src/minimig-aga/agnus_copper.v", 58, "", "Active region did not converge.");
            }
            vlSelfRef.__VactIterCount = ((IData)(1U) 
                                         + vlSelfRef.__VactIterCount);
            vlSelfRef.__VactContinue = 0U;
            if (Vagnus_copper___024root___eval_phase__act(vlSelf)) {
                vlSelfRef.__VactContinue = 1U;
            }
        }
        if (Vagnus_copper___024root___eval_phase__nba(vlSelf)) {
            __VnbaContinue = 1U;
        }
    }
}

#ifdef VL_DEBUG
void Vagnus_copper___024root___eval_debug_assertions(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_debug_assertions\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if (VL_UNLIKELY(((vlSelfRef.clk & 0xfeU)))) {
        Verilated::overWidthError("clk");}
    if (VL_UNLIKELY(((vlSelfRef.clk7_en & 0xfeU)))) {
        Verilated::overWidthError("clk7_en");}
    if (VL_UNLIKELY(((vlSelfRef.reset & 0xfeU)))) {
        Verilated::overWidthError("reset");}
    if (VL_UNLIKELY(((vlSelfRef.ecs & 0xfeU)))) {
        Verilated::overWidthError("ecs");}
    if (VL_UNLIKELY(((vlSelfRef.ackdma & 0xfeU)))) {
        Verilated::overWidthError("ackdma");}
    if (VL_UNLIKELY(((vlSelfRef.enadma & 0xfeU)))) {
        Verilated::overWidthError("enadma");}
    if (VL_UNLIKELY(((vlSelfRef.sof & 0xfeU)))) {
        Verilated::overWidthError("sof");}
    if (VL_UNLIKELY(((vlSelfRef.blit_busy & 0xfeU)))) {
        Verilated::overWidthError("blit_busy");}
    if (VL_UNLIKELY(((vlSelfRef.hpos & 0xfe00U)))) {
        Verilated::overWidthError("hpos");}
}
#endif  // VL_DEBUG
