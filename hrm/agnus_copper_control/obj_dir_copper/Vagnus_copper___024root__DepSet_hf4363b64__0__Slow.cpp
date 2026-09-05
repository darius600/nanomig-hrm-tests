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
