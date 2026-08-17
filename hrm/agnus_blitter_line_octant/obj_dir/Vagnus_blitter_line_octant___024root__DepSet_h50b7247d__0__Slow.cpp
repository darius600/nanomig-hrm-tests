// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_blitter_line_octant.h for the primary calling header

#include "Vagnus_blitter_line_octant__pch.h"
#include "Vagnus_blitter_line_octant___024root.h"

VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___eval_static(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_static\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___eval_initial(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_initial\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___eval_final(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_final\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___dump_triggers__stl(Vagnus_blitter_line_octant___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vagnus_blitter_line_octant___024root___eval_phase__stl(Vagnus_blitter_line_octant___024root* vlSelf);

VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___eval_settle(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_settle\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    IData/*31:0*/ __VstlIterCount;
    CData/*0:0*/ __VstlContinue;
    // Body
    __VstlIterCount = 0U;
    vlSelfRef.__VstlFirstIteration = 1U;
    __VstlContinue = 1U;
    while (__VstlContinue) {
        if (VL_UNLIKELY(((0x64U < __VstlIterCount)))) {
#ifdef VL_DEBUG
            Vagnus_blitter_line_octant___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("agnus_blitter_line_octant.v", 1, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vagnus_blitter_line_octant___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___dump_triggers__stl(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___dump_triggers__stl\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VstlTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

void Vagnus_blitter_line_octant___024root___ico_sequent__TOP__0(Vagnus_blitter_line_octant___024root* vlSelf);

VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___eval_stl(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_stl\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vagnus_blitter_line_octant___024root___ico_sequent__TOP__0(vlSelf);
    }
}

VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___eval_triggers__stl(Vagnus_blitter_line_octant___024root* vlSelf);

VL_ATTR_COLD bool Vagnus_blitter_line_octant___024root___eval_phase__stl(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___eval_phase__stl\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vagnus_blitter_line_octant___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vagnus_blitter_line_octant___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___dump_triggers__ico(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___dump_triggers__ico\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VicoTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VicoTriggered.word(0U))) {
        VL_DBG_MSGF("         'ico' region trigger index 0 is active: Internal 'ico' trigger - first iteration\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___dump_triggers__act(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___dump_triggers__act\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VactTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___dump_triggers__nba(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___dump_triggers__nba\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VnbaTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vagnus_blitter_line_octant___024root___ctor_var_reset(Vagnus_blitter_line_octant___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_blitter_line_octant___024root___ctor_var_reset\n"); );
    Vagnus_blitter_line_octant__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->bltcon1 = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 9501869734275056701ull);
    vlSelf->sign_del = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 274344511746054115ull);
    vlSelf->ash = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 17188495764570791327ull);
    vlSelf->lineinc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18234067433234623263ull);
    vlSelf->linedec = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15641672206508959199ull);
    vlSelf->lineadd = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14984972683649564697ull);
    vlSelf->linesub = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8267424588890652680ull);
}
