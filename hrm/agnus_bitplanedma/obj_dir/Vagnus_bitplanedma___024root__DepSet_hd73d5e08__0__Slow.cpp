// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_bitplanedma.h for the primary calling header

#include "Vagnus_bitplanedma__pch.h"
#include "Vagnus_bitplanedma___024root.h"

VL_ATTR_COLD void Vagnus_bitplanedma___024root___eval_static(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_static\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root___eval_initial(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_initial\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root___eval_final(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_final\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_bitplanedma___024root___dump_triggers__stl(Vagnus_bitplanedma___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vagnus_bitplanedma___024root___eval_phase__stl(Vagnus_bitplanedma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_bitplanedma___024root___eval_settle(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_settle\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus_bitplanedma___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus_bitplanedma.v", 29, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vagnus_bitplanedma___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_bitplanedma___024root___dump_triggers__stl(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___dump_triggers__stl\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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

VL_ATTR_COLD void Vagnus_bitplanedma___024root___stl_sequent__TOP__0(Vagnus_bitplanedma___024root* vlSelf);
VL_ATTR_COLD void Vagnus_bitplanedma___024root____Vm_traceActivitySetAll(Vagnus_bitplanedma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_bitplanedma___024root___eval_stl(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_stl\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vagnus_bitplanedma___024root___stl_sequent__TOP__0(vlSelf);
        Vagnus_bitplanedma___024root____Vm_traceActivitySetAll(vlSelf);
    }
}

extern const VlUnpacked<CData/*7:0*/, 32> Vagnus_bitplanedma__ConstPool__TABLE_h7cec6eda_0;

VL_ATTR_COLD void Vagnus_bitplanedma___024root___stl_sequent__TOP__0(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___stl_sequent__TOP__0\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5 = 0;
    CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6 = 0;
    SData/*14:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16 = 0;
    CData/*0:0*/ agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18;
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18 = 0;
    CData/*4:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    // Body
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17 
        = ((3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))
            ? 4U : ((0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))
                     ? 1U : 2U));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9 
        = ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfend) 
           & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
              >> 2U));
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16 
        = ((1U & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__vdiwstrt) 
                  ^ (IData)(vlSelfRef.vpos))) ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod)
            : (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14 
        = ((4U & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                  << 2U)) | ((2U & ((~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                        >> 1U)) << 1U)) 
                             | (1U & (~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                         >> 2U)))));
    vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12 = 
        ((1U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))) 
         | (2U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))));
    vlSelfRef.agnus_bitplanedma__DOT__shres = ((IData)(vlSelfRef.ecs) 
                                               & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                                                  >> 5U));
    if ((0x4000U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))) {
        vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan 
            = agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16;
        vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan 
            = agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    } else {
        vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan 
            = vlSelfRef.agnus_bitplanedma__DOT__bpl2mod;
        vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan 
            = vlSelfRef.agnus_bitplanedma__DOT__bpl1mod;
    }
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18 
        = ((~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
               >> 4U)) & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__shres)) 
                          & (3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7 
        = ((IData)(vlSelfRef.agnus_bitplanedma__DOT__shres) 
           & (0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))));
    vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11 
        = ((((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
             >> 4U) & (0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))) 
           | ((IData)(vlSelfRef.agnus_bitplanedma__DOT__shres) 
              & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12)));
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5 
        = (1U & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__shres)) 
                 & (~ ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                       >> 4U))));
    agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6 
        = (((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
            & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12)) 
           | (((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
               >> 4U) & (3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
    vlSelfRef.agnus_bitplanedma__DOT__ddfseq_match 
        = (((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18) 
            & (7U == (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq))) 
           | (((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6) 
               & (7U == (0xfU & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))) 
              | ((~ (IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_18)) 
                 & ((~ (IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6)) 
                    & (7U == (7U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))))));
    vlSelfRef.agnus_bitplanedma__DOT__plane = ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                                ? (1U 
                                                   & (~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)))
                                                : ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                    ? 
                                                   ((2U 
                                                     & ((~ (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                                                        << 1U)) 
                                                    | (1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                                           >> 1U))))
                                                    : 
                                                   ((((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
                                                      & (0U 
                                                         == 
                                                         (3U 
                                                          & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))) 
                                                     | ((((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                                                          >> 4U) 
                                                         & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12)) 
                                                        | ((IData)(vlSelfRef.agnus_bitplanedma__DOT__shres) 
                                                           & (3U 
                                                              == 
                                                              (3U 
                                                               & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode))))))
                                                     ? (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14)
                                                     : 
                                                    ((IData)(agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_6)
                                                      ? 
                                                     ((8U 
                                                       & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14))
                                                      : 
                                                     ((0x18U 
                                                       & (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14))))));
    __Vtableidx1 = vlSelfRef.agnus_bitplanedma__DOT__plane;
    vlSelfRef.reg_address_out = Vagnus_bitplanedma__ConstPool__TABLE_h7cec6eda_0
        [__Vtableidx1];
    vlSelfRef.dma = ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfrun) 
                     & (((IData)(vlSelfRef.agnus_bitplanedma__DOT__dmaena_delayed) 
                         >> 1U) & ((IData)(vlSelfRef.hpos) 
                                   & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__plane) 
                                      < (0xfU & ((IData)(vlSelfRef.aga)
                                                  ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)
                                                  : 
                                                 ((7U 
                                                   == 
                                                   (7U 
                                                    & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))
                                                   ? 4U
                                                   : 
                                                  (7U 
                                                   & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))))))));
    vlSelfRef.address_out = ((vlSelfRef.agnus_bitplanedma__DOT__bplpth
                              [(7U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane))] 
                              << 0xfU) | vlSelfRef.agnus_bitplanedma__DOT__bplptl
                             [(7U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane))]);
    vlSelfRef.agnus_bitplanedma__DOT__bplptr_sel = 
        (7U & ((IData)(vlSelfRef.dma) ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane)
                : ((IData)(vlSelfRef.reg_address_in) 
                   >> 1U)));
    vlSelfRef.agnus_bitplanedma__DOT__newpt = (0xfffffU 
                                               & (((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                                    ? 
                                                   ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9) 
                                                    & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                                       >> 1U))
                                                    : 
                                                   ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                     ? (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9)
                                                     : (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfend)))
                                                   ? 
                                                  ((1U 
                                                    & (IData)(vlSelfRef.agnus_bitplanedma__DOT__plane))
                                                    ? 
                                                   (vlSelfRef.address_out 
                                                    + 
                                                    (((0xf8000U 
                                                       & ((- (IData)(
                                                                     (1U 
                                                                      & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan) 
                                                                         >> 0xeU)))) 
                                                          << 0xfU)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan)) 
                                                     + vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17))
                                                    : 
                                                   (vlSelfRef.address_out 
                                                    + 
                                                    (((0xf8000U 
                                                       & ((- (IData)(
                                                                     (1U 
                                                                      & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan) 
                                                                         >> 0xeU)))) 
                                                          << 0xfU)) 
                                                      | (IData)(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan)) 
                                                     + vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17)))
                                                   : 
                                                  (vlSelfRef.address_out 
                                                   + vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17)));
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root___eval_triggers__stl(Vagnus_bitplanedma___024root* vlSelf);

VL_ATTR_COLD bool Vagnus_bitplanedma___024root___eval_phase__stl(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___eval_phase__stl\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vagnus_bitplanedma___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vagnus_bitplanedma___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_bitplanedma___024root___dump_triggers__ico(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___dump_triggers__ico\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vagnus_bitplanedma___024root___dump_triggers__act(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___dump_triggers__act\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VactTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus_bitplanedma___024root___dump_triggers__nba(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___dump_triggers__nba\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VnbaTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vagnus_bitplanedma___024root____Vm_traceActivitySetAll(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root____Vm_traceActivitySetAll\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vm_traceActivity[0U] = 1U;
    vlSelfRef.__Vm_traceActivity[1U] = 1U;
    vlSelfRef.__Vm_traceActivity[2U] = 1U;
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root___ctor_var_reset(Vagnus_bitplanedma___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root___ctor_var_reset\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->clk7_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1674876599471182170ull);
    vlSelf->reset = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9928399931838511862ull);
    vlSelf->harddis = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15113905107406558520ull);
    vlSelf->aga = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5205082699332066466ull);
    vlSelf->ecs = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15277934814765900103ull);
    vlSelf->a1k = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17705640770547906661ull);
    vlSelf->sof = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7496247244052230992ull);
    vlSelf->dmaena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15723610179582842108ull);
    vlSelf->vpos = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 12586444320487401337ull);
    vlSelf->hpos = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2050750607729723554ull);
    vlSelf->hde = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12882197036088020736ull);
    vlSelf->dma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16670614540310729344ull);
    vlSelf->reg_address_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13159730603054898824ull);
    vlSelf->reg_address_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9386968039567798889ull);
    vlSelf->data_in = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10574596302020702150ull);
    vlSelf->address_out = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 16520239430411259709ull);
    vlSelf->agnus_bitplanedma__DOT__ddfstrt = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 3984893569687569025ull);
    vlSelf->agnus_bitplanedma__DOT__ddfstop = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 6709794907757343700ull);
    vlSelf->agnus_bitplanedma__DOT__ddfdiff = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 137503542231210488ull);
    vlSelf->agnus_bitplanedma__DOT__ddfdiff_masked = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 11229693080239644314ull);
    vlSelf->agnus_bitplanedma__DOT__bpl1mod = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 11679072070894260983ull);
    vlSelf->agnus_bitplanedma__DOT__bpl2mod = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 3662201365567328376ull);
    vlSelf->agnus_bitplanedma__DOT__bpl1mod_bscan = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 10142702211631869971ull);
    vlSelf->agnus_bitplanedma__DOT__bpl2mod_bscan = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 11952211883774639991ull);
    vlSelf->agnus_bitplanedma__DOT__bplcon0 = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 7979668509741750550ull);
    vlSelf->agnus_bitplanedma__DOT__bplcon0_delayed = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 4295199725656497421ull);
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        vlSelf->agnus_bitplanedma__DOT__bplcon0_delay[__Vi0] = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 134379895937868223ull);
    }
    vlSelf->agnus_bitplanedma__DOT__fmode = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7960437817900008680ull);
    vlSelf->agnus_bitplanedma__DOT__shres = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5346439855428279472ull);
    vlSelf->agnus_bitplanedma__DOT__newpt = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 9973665631219475164ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus_bitplanedma__DOT__bplpth[__Vi0] = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3349157733830285241ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus_bitplanedma__DOT__bplptl[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 4814665052855574382ull);
    }
    vlSelf->agnus_bitplanedma__DOT__plane = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12886538378495342926ull);
    vlSelf->agnus_bitplanedma__DOT__hardena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8283034287980824120ull);
    vlSelf->agnus_bitplanedma__DOT__softena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5229407603730497277ull);
    vlSelf->agnus_bitplanedma__DOT__ddfena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 273124037734902649ull);
    vlSelf->agnus_bitplanedma__DOT__ddfena_0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3502934776709383121ull);
    vlSelf->agnus_bitplanedma__DOT__ddfseq = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 16027441100961733153ull);
    vlSelf->agnus_bitplanedma__DOT__ddfrun = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14151954007946219937ull);
    vlSelf->agnus_bitplanedma__DOT__ddfend = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1267040480814604096ull);
    vlSelf->agnus_bitplanedma__DOT__dmaena_delayed = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 14729121873718294229ull);
    vlSelf->agnus_bitplanedma__DOT__vdiwstrt = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 4820952368416492712ull);
    vlSelf->agnus_bitplanedma__DOT__vdiwstop = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 13046339129220500081ull);
    vlSelf->agnus_bitplanedma__DOT__vdiwena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8780867760699342850ull);
    vlSelf->agnus_bitplanedma__DOT__hdiwstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17833556513342164290ull);
    vlSelf->agnus_bitplanedma__DOT__hdiwstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 10416462198644152518ull);
    vlSelf->agnus_bitplanedma__DOT__bplptr_sel = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 10472503457578028101ull);
    vlSelf->agnus_bitplanedma__DOT__bp_fmode12 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3680130960809710274ull);
    vlSelf->agnus_bitplanedma__DOT__soft_start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3210467544470974844ull);
    vlSelf->agnus_bitplanedma__DOT__soft_stop = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11407765377965330249ull);
    vlSelf->agnus_bitplanedma__DOT__hard_start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11428523399023506633ull);
    vlSelf->agnus_bitplanedma__DOT__hard_stop = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 735472238955306383ull);
    vlSelf->agnus_bitplanedma__DOT__ddfseq_match = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14653979222263403874ull);
    vlSelf->agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 16236819787908700878ull);
    vlSelf->agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17522548186267676394ull);
    vlSelf->agnus_bitplanedma__DOT__unnamedblk1__DOT__prev_hdiwstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8151935478605647964ull);
    vlSelf->agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 10078670494153537664ull);
    vlSelf->agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 1015586768003836148ull);
    vlSelf->agnus_bitplanedma__DOT__unnamedblk1__DOT__prev_hdiwstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 6188335342349871763ull);
    vlSelf->agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 76474483269314511ull);
    vlSelf->agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15666807457990064349ull);
    vlSelf->agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8428795777926593569ull);
    vlSelf->agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17100277681857337567ull);
    vlSelf->agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_14 = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 17844662916813944023ull);
    vlSelf->agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_17 = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 5060057171769286383ull);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9526919608049418986ull);
    for (int __Vi0 = 0; __Vi0 < 3; ++__Vi0) {
        vlSelf->__Vm_traceActivity[__Vi0] = 0;
    }
}
