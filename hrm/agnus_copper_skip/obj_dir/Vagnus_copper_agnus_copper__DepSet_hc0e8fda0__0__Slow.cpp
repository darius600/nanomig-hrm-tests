// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_copper.h for the primary calling header

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper__Syms.h"
#include "Vagnus_copper_agnus_copper.h"

VL_ATTR_COLD void Vagnus_copper_agnus_copper___stl_sequent__TOP__agnus_copper__0(Vagnus_copper_agnus_copper* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+      Vagnus_copper_agnus_copper___stl_sequent__TOP__agnus_copper__0\n"); );
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
    vlSelfRef.__VdfgRegularize_he23179ad_0_0 = ((IData)(vlSelfRef.__PVT__bus_ena) 
                                                & (IData)(vlSymsp->TOP.clk_ena_dbg));
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
    vlSelfRef.__PVT__enable = ((~ (IData)(vlSelfRef.__PVT__bus_blk)) 
                               & (IData)(vlSelfRef.__VdfgRegularize_he23179ad_0_0));
    vlSelfRef.__PVT__dma_ack = ((IData)(vlSymsp->TOP.ackdma) 
                                & (IData)(vlSelfRef.__PVT__enable));
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
}
