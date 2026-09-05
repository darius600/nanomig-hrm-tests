// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus.h for the primary calling header

#include "Vagnus__pch.h"
#include "Vagnus__Syms.h"
#include "Vagnus___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus___024root___dump_triggers__ico(Vagnus___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus___024root___eval_triggers__ico(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_triggers__ico\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VicoTriggered.setBit(0U, (IData)(vlSelfRef.__VicoFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vagnus___024root___dump_triggers__ico(vlSelf);
    }
#endif
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus___024root___dump_triggers__act(Vagnus___024root* vlSelf);
#endif  // VL_DEBUG

void Vagnus___024root___eval_triggers__act(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_triggers__act\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VactTriggered.setBit(0U, ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry) 
                                          != (IData)(vlSelfRef.__Vtrigprevexpr___TOP__agnus__DOT__bl1__DOT__bltfl1__DOT__carry__1)));
    vlSelfRef.__VactTriggered.setBit(1U, ((IData)(vlSelfRef.clk) 
                                          & (~ (IData)(vlSelfRef.__Vtrigprevexpr___TOP__clk__0))));
    vlSelfRef.__Vtrigprevexpr___TOP__agnus__DOT__bl1__DOT__bltfl1__DOT__carry__1 
        = vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry;
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
    if (VL_UNLIKELY(((1U & (~ (IData)(vlSelfRef.__VactDidInit)))))) {
        vlSelfRef.__VactDidInit = 1U;
        vlSelfRef.__VactTriggered.setBit(0U, 1U);
    }
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vagnus___024root___dump_triggers__act(vlSelf);
    }
#endif
}
