// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_copper.h for the primary calling header

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper__Syms.h"
#include "Vagnus_copper___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_copper___024root___dump_triggers__stl(Vagnus_copper___024root* vlSelf);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vagnus_copper___024root___eval_triggers__stl(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_triggers__stl\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered.setBit(0U, (IData)(vlSelfRef.__VstlFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vagnus_copper___024root___dump_triggers__stl(vlSelf);
    }
#endif
}

VL_ATTR_COLD void Vagnus_copper___024root___stl_sequent__TOP__0(Vagnus_copper___024root* vlSelf);
VL_ATTR_COLD void Vagnus_copper___024root____Vm_traceActivitySetAll(Vagnus_copper___024root* vlSelf);
VL_ATTR_COLD void Vagnus_copper_agnus_copper___stl_sequent__TOP__agnus_copper__0(Vagnus_copper_agnus_copper* vlSelf);
VL_ATTR_COLD void Vagnus_copper___024root___stl_sequent__TOP__1(Vagnus_copper___024root* vlSelf);

VL_ATTR_COLD void Vagnus_copper___024root___eval_stl(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___eval_stl\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vagnus_copper___024root___stl_sequent__TOP__0(vlSelf);
        Vagnus_copper___024root____Vm_traceActivitySetAll(vlSelf);
        Vagnus_copper_agnus_copper___stl_sequent__TOP__agnus_copper__0((&vlSymsp->TOP__agnus_copper));
        Vagnus_copper___024root___stl_sequent__TOP__1(vlSelf);
    }
}

VL_ATTR_COLD void Vagnus_copper___024root___stl_sequent__TOP__0(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___stl_sequent__TOP__0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.bus_ena_dbg = vlSymsp->TOP__agnus_copper.__PVT__bus_ena;
    vlSelfRef.address_out = vlSymsp->TOP__agnus_copper.address_out;
    vlSelfRef.copper_state_dbg = vlSymsp->TOP__agnus_copper.__PVT__copper_state;
    vlSelfRef.clk_ena_dbg = (1U & (IData)(vlSelfRef.hpos));
}

VL_ATTR_COLD void Vagnus_copper___024root___stl_sequent__TOP__1(Vagnus_copper___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root___stl_sequent__TOP__1\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
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
