// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_copper.h for the primary calling header

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper__Syms.h"
#include "Vagnus_copper_agnus_copper.h"

VL_INLINE_OPT void Vagnus_copper_agnus_copper___ico_sequent__TOP__agnus_copper__0(Vagnus_copper_agnus_copper* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+      Vagnus_copper_agnus_copper___ico_sequent__TOP__agnus_copper__0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__PVT__beam_match_skip = ((IData)(vlSelfRef.__PVT__beam_match) 
                                        & ((~ (IData)(vlSymsp->TOP.blit_busy)) 
                                           | ((IData)(vlSelfRef.__PVT__ir2) 
                                              >> 0xfU)));
    vlSelfRef.__PVT__illegalreg = (((~ (IData)(vlSelfRef.__PVT__cdang)) 
                                    & (0U == (0x180U 
                                              & (IData)(vlSelfRef.__PVT__ir2)))) 
                                   | ((~ (IData)(vlSymsp->TOP.ecs)) 
                                      & (0U == (0x1c0U 
                                                & (IData)(vlSelfRef.__PVT__ir2)))));
    vlSelfRef.dma_req = (1U & ((4U & (IData)(vlSelfRef.__PVT__copper_state))
                                ? ((2U & (IData)(vlSelfRef.__PVT__copper_state))
                                    ? ((1U & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                       && (1U & (~ 
                                                 ((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                  & (IData)(vlSelfRef.__PVT__illegalreg)))))
                                    : (~ (IData)(vlSelfRef.__PVT__copper_state)))
                                : ((1U & (~ ((IData)(vlSelfRef.__PVT__copper_state) 
                                             >> 1U))) 
                                   && (1U & (~ (IData)(vlSelfRef.__PVT__copper_state))))));
    vlSelfRef.selreg = ((1U & ((IData)(vlSelfRef.__PVT__copper_state) 
                               >> 2U)) && ((1U & ((IData)(vlSelfRef.__PVT__copper_state) 
                                                  >> 1U)) 
                                           && ((1U 
                                                & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                               && ((1U 
                                                    & (~ 
                                                       ((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                        & (IData)(vlSelfRef.__PVT__illegalreg)))) 
                                                   && ((1U 
                                                        & (~ 
                                                           ((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                            & (IData)(vlSelfRef.__PVT__skip_flag)))) 
                                                       && (1U 
                                                           & (~ (IData)(vlSelfRef.__PVT__ir2))))))));
    vlSelfRef.selins = ((1U & ((IData)(vlSelfRef.__PVT__copper_state) 
                               >> 2U)) && (1U & ((2U 
                                                  & (IData)(vlSelfRef.__PVT__copper_state))
                                                  ? 
                                                 ((1U 
                                                   & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                           & (IData)(vlSelfRef.__PVT__illegalreg)))) 
                                                      && (((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                           & (IData)(vlSelfRef.__PVT__skip_flag)) 
                                                          || (1U 
                                                              & (IData)(vlSelfRef.__PVT__ir2)))))
                                                  : 
                                                 (~ (IData)(vlSelfRef.__PVT__copper_state)))));
}

VL_INLINE_OPT void Vagnus_copper_agnus_copper___ico_sequent__TOP__agnus_copper__1(Vagnus_copper_agnus_copper* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+      Vagnus_copper_agnus_copper___ico_sequent__TOP__agnus_copper__1\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VdfgRegularize_he23179ad_0_0 = ((IData)(vlSelfRef.__PVT__bus_ena) 
                                                & (IData)(vlSymsp->TOP.clk_ena_dbg));
    vlSelfRef.__PVT__enable = ((~ (IData)(vlSelfRef.__PVT__bus_blk)) 
                               & (IData)(vlSelfRef.__VdfgRegularize_he23179ad_0_0));
    vlSelfRef.__PVT__dma_ack = ((IData)(vlSymsp->TOP.ackdma) 
                                & (IData)(vlSelfRef.__PVT__enable));
    if ((4U & (IData)(vlSelfRef.__PVT__copper_state))) {
        if ((2U & (IData)(vlSelfRef.__PVT__copper_state))) {
            vlSelfRef.__PVT__skip = ((1U & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                     && ((1U & (~ (
                                                   (~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                   & (IData)(vlSelfRef.__PVT__illegalreg)))) 
                                         && (((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                              & (IData)(vlSelfRef.__PVT__skip_flag)) 
                                             && (1U 
                                                 & (~ (IData)(vlSelfRef.__PVT__dma_ack))))));
            vlSelfRef.__PVT__copper_next = ((1U & (IData)(vlSelfRef.__PVT__copper_state))
                                             ? 4U : 
                                            (((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                              & (IData)(vlSelfRef.__PVT__illegalreg))
                                              ? 6U : 
                                             (((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                               & (IData)(vlSelfRef.__PVT__skip_flag))
                                               ? ((IData)(vlSelfRef.__PVT__dma_ack)
                                                   ? 4U
                                                   : 6U)
                                               : ((1U 
                                                   & (IData)(vlSelfRef.__PVT__ir2))
                                                   ? 
                                                  ((IData)(vlSelfRef.__PVT__dma_ack)
                                                    ? 2U
                                                    : 6U)
                                                   : 
                                                  ((IData)(vlSelfRef.__PVT__dma_ack)
                                                    ? 4U
                                                    : 6U)))));
        } else {
            vlSelfRef.__PVT__skip = ((1U & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                     && (IData)(vlSelfRef.__PVT__skip_flag));
            vlSelfRef.__PVT__copper_next = ((1U & (IData)(vlSelfRef.__PVT__copper_state))
                                             ? 4U : 
                                            ((IData)(vlSelfRef.__PVT__dma_ack)
                                              ? 6U : 4U));
        }
    } else {
        vlSelfRef.__PVT__skip = ((1U & ((IData)(vlSelfRef.__PVT__copper_state) 
                                        >> 1U)) && 
                                 ((1U & (IData)(vlSelfRef.__PVT__copper_state)) 
                                  && ((1U & (IData)(vlSelfRef.__PVT__ir2)) 
                                      && (IData)(vlSelfRef.__PVT__beam_match_skip))));
        vlSelfRef.__PVT__copper_next = ((2U & (IData)(vlSelfRef.__PVT__copper_state))
                                         ? ((1U & (IData)(vlSelfRef.__PVT__copper_state))
                                             ? ((1U 
                                                 & (IData)(vlSelfRef.__PVT__ir2))
                                                 ? 
                                                ((IData)(vlSelfRef.__PVT__beam_match_skip)
                                                  ? 
                                                 ((IData)(vlSymsp->TOP.enadma)
                                                   ? 4U
                                                   : 3U)
                                                  : 
                                                 ((IData)(vlSymsp->TOP.enadma)
                                                   ? 4U
                                                   : 3U))
                                                 : 
                                                ((IData)(vlSelfRef.__PVT__beam_match_wait)
                                                  ? 
                                                 ((IData)(vlSymsp->TOP.enadma)
                                                   ? 4U
                                                   : 3U)
                                                  : 3U))
                                             : ((IData)(vlSymsp->TOP.enadma)
                                                 ? 3U
                                                 : 2U))
                                         : ((1U & (IData)(vlSelfRef.__PVT__copper_state))
                                             ? 4U : 
                                            ((IData)(vlSelfRef.__PVT__dma_ack)
                                              ? 4U : 0U)));
    }
}

VL_INLINE_OPT void Vagnus_copper_agnus_copper___nba_sequent__TOP__agnus_copper__0(Vagnus_copper_agnus_copper* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+      Vagnus_copper_agnus_copper___nba_sequent__TOP__agnus_copper__0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*19:0*/ __Vdly__address_out;
    __Vdly__address_out = 0;
    // Body
    __Vdly__address_out = vlSelfRef.address_out;
    if (vlSymsp->TOP.clk7_en) {
        if ((((IData)(vlSelfRef.__PVT__dma_ack) & (IData)(vlSelfRef.__PVT__strobe1)) 
             & (0U == (IData)(vlSelfRef.__PVT__copper_state)))) {
            __Vdly__address_out = (((IData)(vlSelfRef.__PVT__cop1lch) 
                                    << 0xfU) | (IData)(vlSelfRef.__PVT__cop1lcl));
        } else if ((((IData)(vlSelfRef.__PVT__dma_ack) 
                     & (IData)(vlSelfRef.__PVT__strobe2)) 
                    & (0U == (IData)(vlSelfRef.__PVT__copper_state)))) {
            __Vdly__address_out = (((IData)(vlSelfRef.__PVT__cop2lch) 
                                    << 0xfU) | (IData)(vlSelfRef.__PVT__cop2lcl));
        } else if (((IData)(vlSelfRef.__PVT__dma_ack) 
                    & ((IData)(vlSelfRef.selins) | (IData)(vlSelfRef.selreg)))) {
            __Vdly__address_out = (0xfffffU & ((IData)(1U) 
                                               + vlSelfRef.address_out));
        }
        if (vlSelfRef.__PVT__enable) {
            vlSelfRef.__PVT__skip_flag = vlSelfRef.__PVT__skip;
        }
        if (vlSymsp->TOP.reset) {
            vlSelfRef.__PVT__cdang = 0U;
            vlSelfRef.__PVT__cop1lch = 0U;
            vlSelfRef.__PVT__cop1lcl = 0U;
            vlSelfRef.__PVT__cop2lch = 0U;
            vlSelfRef.__PVT__cop2lcl = 0U;
        } else {
            if ((0x17U == (IData)(vlSymsp->TOP.reg_address_in))) {
                vlSelfRef.__PVT__cdang = (1U & ((IData)(vlSymsp->TOP.data_in) 
                                                >> 1U));
            }
            if ((0x40U == (IData)(vlSymsp->TOP.reg_address_in))) {
                vlSelfRef.__PVT__cop1lch = (0x1fU & (IData)(vlSymsp->TOP.data_in));
            }
            if ((0x41U == (IData)(vlSymsp->TOP.reg_address_in))) {
                vlSelfRef.__PVT__cop1lcl = (0x7fffU 
                                            & ((IData)(vlSymsp->TOP.data_in) 
                                               >> 1U));
            }
            if ((0x42U == (IData)(vlSymsp->TOP.reg_address_in))) {
                vlSelfRef.__PVT__cop2lch = (0x1fU & (IData)(vlSymsp->TOP.data_in));
            }
            if ((0x43U == (IData)(vlSymsp->TOP.reg_address_in))) {
                vlSelfRef.__PVT__cop2lcl = (0x7fffU 
                                            & ((IData)(vlSymsp->TOP.data_in) 
                                               >> 1U));
            }
        }
        if (((IData)(vlSelfRef.__PVT__copjmp2) & (IData)(vlSymsp->TOP.clk_ena_dbg))) {
            vlSelfRef.__PVT__strobe2 = 1U;
        } else if (((0U == (IData)(vlSelfRef.__PVT__copper_state)) 
                    & (IData)(vlSelfRef.__PVT__dma_ack))) {
            vlSelfRef.__PVT__strobe2 = 0U;
        }
        if (((IData)(vlSelfRef.__PVT__copjmp1) & (IData)(vlSymsp->TOP.clk_ena_dbg))) {
            vlSelfRef.__PVT__strobe1 = 1U;
        } else if (((0U == (IData)(vlSelfRef.__PVT__copper_state)) 
                    & (IData)(vlSelfRef.__PVT__dma_ack))) {
            vlSelfRef.__PVT__strobe1 = 0U;
        }
        if (vlSymsp->TOP.clk_ena_dbg) {
            vlSelfRef.__PVT__bus_ena = (1U & ((IData)(vlSelfRef.__PVT__bus_blk) 
                                              | (~ (IData)(vlSelfRef.__PVT__bus_ena))));
            vlSelfRef.__PVT__beam_match_wait = vlSelfRef.__PVT__beam_match_skip;
            vlSelfRef.__PVT__beam_match = ((((IData)(vlSymsp->TOP.vpos) 
                                             << 7U) 
                                            | (0x7fU 
                                               & ((IData)(vlSymsp->TOP.hpos) 
                                                  >> 2U))) 
                                           >= (((((0x4000U 
                                                   & (IData)(vlSelfRef.__PVT__ir1)) 
                                                  | (0x2000U 
                                                     & (((0x4000U 
                                                          & (IData)(vlSelfRef.__PVT__ir2))
                                                          ? 
                                                         ((IData)(vlSelfRef.__PVT__ir1) 
                                                          >> 0xdU)
                                                          : 
                                                         ((IData)(vlSymsp->TOP.vpos) 
                                                          >> 6U)) 
                                                        << 0xdU))) 
                                                 | ((0x1000U 
                                                     & (((0x2000U 
                                                          & (IData)(vlSelfRef.__PVT__ir2))
                                                          ? 
                                                         ((IData)(vlSelfRef.__PVT__ir1) 
                                                          >> 0xcU)
                                                          : 
                                                         ((IData)(vlSymsp->TOP.vpos) 
                                                          >> 5U)) 
                                                        << 0xcU)) 
                                                    | (0x800U 
                                                       & (((0x1000U 
                                                            & (IData)(vlSelfRef.__PVT__ir2))
                                                            ? 
                                                           ((IData)(vlSelfRef.__PVT__ir1) 
                                                            >> 0xbU)
                                                            : 
                                                           ((IData)(vlSymsp->TOP.vpos) 
                                                            >> 4U)) 
                                                          << 0xbU)))) 
                                                | (((0x400U 
                                                     & (((0x800U 
                                                          & (IData)(vlSelfRef.__PVT__ir2))
                                                          ? 
                                                         ((IData)(vlSelfRef.__PVT__ir1) 
                                                          >> 0xaU)
                                                          : 
                                                         ((IData)(vlSymsp->TOP.vpos) 
                                                          >> 3U)) 
                                                        << 0xaU)) 
                                                    | (0x200U 
                                                       & (((0x400U 
                                                            & (IData)(vlSelfRef.__PVT__ir2))
                                                            ? 
                                                           ((IData)(vlSelfRef.__PVT__ir1) 
                                                            >> 9U)
                                                            : 
                                                           ((IData)(vlSymsp->TOP.vpos) 
                                                            >> 2U)) 
                                                          << 9U))) 
                                                   | ((0x100U 
                                                       & (((0x200U 
                                                            & (IData)(vlSelfRef.__PVT__ir2))
                                                            ? 
                                                           ((IData)(vlSelfRef.__PVT__ir1) 
                                                            >> 8U)
                                                            : 
                                                           ((IData)(vlSymsp->TOP.vpos) 
                                                            >> 1U)) 
                                                          << 8U)) 
                                                      | (0x80U 
                                                         & (((0x100U 
                                                              & (IData)(vlSelfRef.__PVT__ir2))
                                                              ? 
                                                             ((IData)(vlSelfRef.__PVT__ir1) 
                                                              >> 7U)
                                                              : (IData)(vlSymsp->TOP.vpos)) 
                                                            << 7U))))) 
                                               | ((((0x40U 
                                                     & (((0x80U 
                                                          & (IData)(vlSelfRef.__PVT__ir2))
                                                          ? 
                                                         ((IData)(vlSelfRef.__PVT__ir1) 
                                                          >> 6U)
                                                          : 
                                                         ((IData)(vlSymsp->TOP.hpos) 
                                                          >> 8U)) 
                                                        << 6U)) 
                                                    | (0x20U 
                                                       & (((0x40U 
                                                            & (IData)(vlSelfRef.__PVT__ir2))
                                                            ? 
                                                           ((IData)(vlSelfRef.__PVT__ir1) 
                                                            >> 5U)
                                                            : 
                                                           ((IData)(vlSymsp->TOP.hpos) 
                                                            >> 7U)) 
                                                          << 5U))) 
                                                   | ((0x10U 
                                                       & (((0x20U 
                                                            & (IData)(vlSelfRef.__PVT__ir2))
                                                            ? 
                                                           ((IData)(vlSelfRef.__PVT__ir1) 
                                                            >> 4U)
                                                            : 
                                                           ((IData)(vlSymsp->TOP.hpos) 
                                                            >> 6U)) 
                                                          << 4U)) 
                                                      | (8U 
                                                         & (((0x10U 
                                                              & (IData)(vlSelfRef.__PVT__ir2))
                                                              ? 
                                                             ((IData)(vlSelfRef.__PVT__ir1) 
                                                              >> 3U)
                                                              : 
                                                             ((IData)(vlSymsp->TOP.hpos) 
                                                              >> 5U)) 
                                                            << 3U)))) 
                                                  | ((4U 
                                                      & (((8U 
                                                           & (IData)(vlSelfRef.__PVT__ir2))
                                                           ? 
                                                          ((IData)(vlSelfRef.__PVT__ir1) 
                                                           >> 2U)
                                                           : 
                                                          ((IData)(vlSymsp->TOP.hpos) 
                                                           >> 4U)) 
                                                         << 2U)) 
                                                     | ((2U 
                                                         & (((4U 
                                                              & (IData)(vlSelfRef.__PVT__ir2))
                                                              ? 
                                                             ((IData)(vlSelfRef.__PVT__ir1) 
                                                              >> 1U)
                                                              : 
                                                             ((IData)(vlSymsp->TOP.hpos) 
                                                              >> 3U)) 
                                                            << 1U)) 
                                                        | (1U 
                                                           & ((2U 
                                                               & (IData)(vlSelfRef.__PVT__ir2))
                                                               ? (IData)(vlSelfRef.__PVT__ir1)
                                                               : 
                                                              ((IData)(vlSymsp->TOP.hpos) 
                                                               >> 2U))))))));
        }
        if ((0x46U == (IData)(vlSymsp->TOP.reg_address_in))) {
            vlSelfRef.__PVT__ir1 = (0x7fffU & ((IData)(vlSelfRef.__PVT__ir2) 
                                               >> 1U));
            vlSelfRef.__PVT__ir2 = vlSymsp->TOP.data_in;
        }
        if (((IData)(vlSymsp->TOP.reset) | ((IData)(vlSymsp->TOP.clk_ena_dbg) 
                                            & (IData)(vlSelfRef.__PVT__strobe)))) {
            vlSelfRef.__PVT__copper_state = 0U;
        } else if (vlSelfRef.__PVT__enable) {
            vlSelfRef.__PVT__copper_state = vlSelfRef.__PVT__copper_next;
        }
        if (vlSymsp->TOP.clk_ena_dbg) {
            vlSelfRef.__PVT__bus_blk = (1U == (0xffU 
                                               & ((IData)(vlSymsp->TOP.hpos) 
                                                  >> 1U)));
            vlSelfRef.__PVT__strobe = ((IData)(vlSelfRef.__PVT__copjmp1) 
                                       | (IData)(vlSelfRef.__PVT__copjmp2));
        }
        if ((0x45U == (IData)(vlSymsp->TOP.reg_address_in))) {
            vlSelfRef.__PVT__copjmp2 = 1U;
        } else if (vlSymsp->TOP.clk_ena_dbg) {
            vlSelfRef.__PVT__copjmp2 = 0U;
        }
        if (((0x44U == (IData)(vlSymsp->TOP.reg_address_in)) 
             | (IData)(vlSymsp->TOP.sof))) {
            vlSelfRef.__PVT__copjmp1 = 1U;
        } else if (vlSymsp->TOP.clk_ena_dbg) {
            vlSelfRef.__PVT__copjmp1 = 0U;
        }
    }
    vlSelfRef.address_out = __Vdly__address_out;
    vlSelfRef.__VdfgRegularize_he23179ad_0_0 = ((IData)(vlSelfRef.__PVT__bus_ena) 
                                                & (IData)(vlSymsp->TOP.clk_ena_dbg));
    vlSelfRef.__PVT__enable = ((~ (IData)(vlSelfRef.__PVT__bus_blk)) 
                               & (IData)(vlSelfRef.__VdfgRegularize_he23179ad_0_0));
    vlSelfRef.__PVT__dma_ack = ((IData)(vlSymsp->TOP.ackdma) 
                                & (IData)(vlSelfRef.__PVT__enable));
    vlSelfRef.__PVT__beam_match_skip = ((IData)(vlSelfRef.__PVT__beam_match) 
                                        & ((~ (IData)(vlSymsp->TOP.blit_busy)) 
                                           | ((IData)(vlSelfRef.__PVT__ir2) 
                                              >> 0xfU)));
    vlSelfRef.__PVT__illegalreg = (((~ (IData)(vlSelfRef.__PVT__cdang)) 
                                    & (0U == (0x180U 
                                              & (IData)(vlSelfRef.__PVT__ir2)))) 
                                   | ((~ (IData)(vlSymsp->TOP.ecs)) 
                                      & (0U == (0x1c0U 
                                                & (IData)(vlSelfRef.__PVT__ir2)))));
    if ((4U & (IData)(vlSelfRef.__PVT__copper_state))) {
        if ((2U & (IData)(vlSelfRef.__PVT__copper_state))) {
            vlSelfRef.dma_req = (1U & ((1U & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                       && (1U & (~ 
                                                 ((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                  & (IData)(vlSelfRef.__PVT__illegalreg))))));
            vlSelfRef.__PVT__skip = ((1U & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                     && ((1U & (~ (
                                                   (~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                   & (IData)(vlSelfRef.__PVT__illegalreg)))) 
                                         && (((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                              & (IData)(vlSelfRef.__PVT__skip_flag)) 
                                             && (1U 
                                                 & (~ (IData)(vlSelfRef.__PVT__dma_ack))))));
            vlSelfRef.__PVT__copper_next = ((1U & (IData)(vlSelfRef.__PVT__copper_state))
                                             ? 4U : 
                                            (((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                              & (IData)(vlSelfRef.__PVT__illegalreg))
                                              ? 6U : 
                                             (((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                               & (IData)(vlSelfRef.__PVT__skip_flag))
                                               ? ((IData)(vlSelfRef.__PVT__dma_ack)
                                                   ? 4U
                                                   : 6U)
                                               : ((1U 
                                                   & (IData)(vlSelfRef.__PVT__ir2))
                                                   ? 
                                                  ((IData)(vlSelfRef.__PVT__dma_ack)
                                                    ? 2U
                                                    : 6U)
                                                   : 
                                                  ((IData)(vlSelfRef.__PVT__dma_ack)
                                                    ? 4U
                                                    : 6U)))));
        } else {
            vlSelfRef.dma_req = (1U & (~ (IData)(vlSelfRef.__PVT__copper_state)));
            vlSelfRef.__PVT__skip = ((1U & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                     && (IData)(vlSelfRef.__PVT__skip_flag));
            vlSelfRef.__PVT__copper_next = ((1U & (IData)(vlSelfRef.__PVT__copper_state))
                                             ? 4U : 
                                            ((IData)(vlSelfRef.__PVT__dma_ack)
                                              ? 6U : 4U));
        }
    } else {
        vlSelfRef.dma_req = (1U & ((1U & (~ ((IData)(vlSelfRef.__PVT__copper_state) 
                                             >> 1U))) 
                                   && (1U & (~ (IData)(vlSelfRef.__PVT__copper_state)))));
        vlSelfRef.__PVT__skip = ((1U & ((IData)(vlSelfRef.__PVT__copper_state) 
                                        >> 1U)) && 
                                 ((1U & (IData)(vlSelfRef.__PVT__copper_state)) 
                                  && ((1U & (IData)(vlSelfRef.__PVT__ir2)) 
                                      && (IData)(vlSelfRef.__PVT__beam_match_skip))));
        vlSelfRef.__PVT__copper_next = ((2U & (IData)(vlSelfRef.__PVT__copper_state))
                                         ? ((1U & (IData)(vlSelfRef.__PVT__copper_state))
                                             ? ((1U 
                                                 & (IData)(vlSelfRef.__PVT__ir2))
                                                 ? 
                                                ((IData)(vlSelfRef.__PVT__beam_match_skip)
                                                  ? 
                                                 ((IData)(vlSymsp->TOP.enadma)
                                                   ? 4U
                                                   : 3U)
                                                  : 
                                                 ((IData)(vlSymsp->TOP.enadma)
                                                   ? 4U
                                                   : 3U))
                                                 : 
                                                ((IData)(vlSelfRef.__PVT__beam_match_wait)
                                                  ? 
                                                 ((IData)(vlSymsp->TOP.enadma)
                                                   ? 4U
                                                   : 3U)
                                                  : 3U))
                                             : ((IData)(vlSymsp->TOP.enadma)
                                                 ? 3U
                                                 : 2U))
                                         : ((1U & (IData)(vlSelfRef.__PVT__copper_state))
                                             ? 4U : 
                                            ((IData)(vlSelfRef.__PVT__dma_ack)
                                              ? 4U : 0U)));
    }
    vlSelfRef.selreg = ((1U & ((IData)(vlSelfRef.__PVT__copper_state) 
                               >> 2U)) && ((1U & ((IData)(vlSelfRef.__PVT__copper_state) 
                                                  >> 1U)) 
                                           && ((1U 
                                                & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                               && ((1U 
                                                    & (~ 
                                                       ((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                        & (IData)(vlSelfRef.__PVT__illegalreg)))) 
                                                   && ((1U 
                                                        & (~ 
                                                           ((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                            & (IData)(vlSelfRef.__PVT__skip_flag)))) 
                                                       && (1U 
                                                           & (~ (IData)(vlSelfRef.__PVT__ir2))))))));
    vlSelfRef.selins = ((1U & ((IData)(vlSelfRef.__PVT__copper_state) 
                               >> 2U)) && (1U & ((2U 
                                                  & (IData)(vlSelfRef.__PVT__copper_state))
                                                  ? 
                                                 ((1U 
                                                   & (~ (IData)(vlSelfRef.__PVT__copper_state))) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                           & (IData)(vlSelfRef.__PVT__illegalreg)))) 
                                                      && (((~ (IData)(vlSelfRef.__PVT__ir2)) 
                                                           & (IData)(vlSelfRef.__PVT__skip_flag)) 
                                                          || (1U 
                                                              & (IData)(vlSelfRef.__PVT__ir2)))))
                                                  : 
                                                 (~ (IData)(vlSelfRef.__PVT__copper_state)))));
}
