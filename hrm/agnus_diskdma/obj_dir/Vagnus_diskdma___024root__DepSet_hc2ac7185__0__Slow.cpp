// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_diskdma.h for the primary calling header

#include "Vagnus_diskdma__pch.h"
#include "Vagnus_diskdma__Syms.h"
#include "Vagnus_diskdma___024root.h"

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_diskdma___024root___dump_triggers__stl(Vagnus_diskdma___024root* vlSelf);
#endif  // VL_DEBUG

VL_ATTR_COLD void Vagnus_diskdma___024root___eval_triggers__stl(Vagnus_diskdma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_diskdma___024root___eval_triggers__stl\n"); );
    Vagnus_diskdma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__VstlTriggered.setBit(0U, (IData)(vlSelfRef.__VstlFirstIteration));
#ifdef VL_DEBUG
    if (VL_UNLIKELY(vlSymsp->_vm_contextp__->debug())) {
        Vagnus_diskdma___024root___dump_triggers__stl(vlSelf);
    }
#endif
}
