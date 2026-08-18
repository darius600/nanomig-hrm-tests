// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_copper.h for the primary calling header

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper__Syms.h"
#include "Vagnus_copper___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__ico(Vagnus_copper___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_copper___024root___eval_triggers__ico(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_triggers__ico\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered.setBit(0U, (IData)(vlSelfRef.__VicoFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vagnus_copper___024root___dump_triggers__ico(vlSelf);
    }
#endif
}

void Vagnus_copper_agnus_copper___ico_sequent__TOP__agnus_copper__0(Vagnus_copper_agnus_copper* vlSelf);
void Vagnus_copper___024root___ico_sequent__TOP__0(Vagnus_copper___024root* vlSelf);
void Vagnus_copper_agnus_copper___ico_sequent__TOP__agnus_copper__1(Vagnus_copper_agnus_copper* vlSelf);
void Vagnus_copper___024root___ico_sequent__TOP__1(Vagnus_copper___024root* vlSelf);

void Vagnus_copper___024root___eval_ico(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_ico\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        Vagnus_copper_agnus_copper___ico_sequent__TOP__agnus_copper__0((&vlSymsp->TOP__agnus_copper));
        vlSelfRef.__Vm_traceActivity[1U] = 1U;
        Vagnus_copper___024root___ico_sequent__TOP__0(vlSelf);
        Vagnus_copper_agnus_copper___ico_sequent__TOP__agnus_copper__1((&vlSymsp->TOP__agnus_copper));
        Vagnus_copper___024root___ico_sequent__TOP__1(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_copper___024root___ico_sequent__TOP__0(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___ico_sequent__TOP__0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.clk_ena_dbg = (1U & (IData)(vlSelfRef.hpos));
    vlSelfRef.dma_req_dbg = vlSymsp->TOP__agnus_copper.dma_req;
}

VL_INLINE_OPT void Vagnus_copper___024root___ico_sequent__TOP__1(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___ico_sequent__TOP__1\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.reqdma = ((IData)(vlSymsp->TOP__agnus_copper.dma_req) 
                        & (IData)(vlSymsp->TOP__agnus_copper.__VdfgRegularize_he23179ad_0_0));
    vlSelfRef.reg_address_out = (((IData)(vlSymsp->TOP__agnus_copper.__PVT__enable) 
                                  & (IData)(vlSymsp->TOP__agnus_copper.selins))
                                  ? 0x46U : (((IData)(vlSymsp->TOP__agnus_copper.__PVT__enable) 
                                              & (IData)(vlSymsp->TOP__agnus_copper.selreg))
                                              ? (0xffU 
                                                 & ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2) 
                                                    >> 1U))
                                              : 0xffU));
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__act(Vagnus_copper___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus_copper___024root___eval_triggers__act(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_triggers__act\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered.setBit(0U, ((IData)(vlSelfRef.clk) 
                                          & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__0))));
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vagnus_copper___024root___dump_triggers__act(vlSelf);
    }
#endif
}

void Vagnus_copper_agnus_copper___nba_sequent__TOP__agnus_copper__0(Vagnus_copper_agnus_copper* vlSelf);
void Vagnus_copper___024root___nba_sequent__TOP__0(Vagnus_copper___024root* vlSelf);

void Vagnus_copper___024root___eval_nba(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_nba\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        Vagnus_copper_agnus_copper___nba_sequent__TOP__agnus_copper__0((&vlSymsp->TOP__agnus_copper));
        vlSelfRef.__Vm_traceActivity[2U] = 1U;
        Vagnus_copper___024root___nba_sequent__TOP__0(vlSelf);
    }
}

VL_INLINE_OPT void Vagnus_copper___024root___nba_sequent__TOP__0(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___nba_sequent__TOP__0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.bus_ena_dbg = vlSymsp->TOP__agnus_copper.__PVT__bus_ena;
    vlSelfRef.address_out = vlSymsp->TOP__agnus_copper.address_out;
    vlSelfRef.copper_state_dbg = vlSymsp->TOP__agnus_copper.__PVT__copper_state;
    vlSelfRef.dma_req_dbg = vlSymsp->TOP__agnus_copper.dma_req;
    vlSelfRef.reqdma = ((IData)(vlSymsp->TOP__agnus_copper.dma_req) 
                        & (IData)(vlSymsp->TOP__agnus_copper.__VdfgRegularize_he23179ad_0_0));
    vlSelfRef.reg_address_out = (((IData)(vlSymsp->TOP__agnus_copper.__PVT__enable) 
                                  & (IData)(vlSymsp->TOP__agnus_copper.selins))
                                  ? 0x46U : (((IData)(vlSymsp->TOP__agnus_copper.__PVT__enable) 
                                              & (IData)(vlSymsp->TOP__agnus_copper.selreg))
                                              ? (0xffU 
                                                 & ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2) 
                                                    >> 1U))
                                              : 0xffU));
}
