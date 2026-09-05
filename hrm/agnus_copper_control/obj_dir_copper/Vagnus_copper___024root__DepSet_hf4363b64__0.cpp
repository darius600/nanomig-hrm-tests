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
