// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus.h for the primary calling header

#include "Vagnus__pch.h"
#include "Vagnus___024root.h"

VL_ATTR_COLD void Vagnus___024root___eval_static(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_static\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    vlSelfRef.__Vtrigprevexpr___TOP__agnus__DOT__bl1__DOT__bltfl1__DOT__carry__0 
        = vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry;
    vlSelfRef.__Vtrigprevexpr___TOP__agnus__DOT__bl1__DOT__bltfl1__DOT__carry__1 
        = vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry;
    vlSelfRef.__Vtrigprevexpr___TOP__clk__0 = vlSelfRef.clk;
}

VL_ATTR_COLD void Vagnus___024root___eval_initial(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_initial\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

VL_ATTR_COLD void Vagnus___024root___eval_final(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_final\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus___024root___dump_triggers__stl(Vagnus___024root* vlSelf);
#endif  // VL_DEBUG
VL_ATTR_COLD bool Vagnus___024root___eval_phase__stl(Vagnus___024root* vlSelf);

VL_ATTR_COLD void Vagnus___024root___eval_settle(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_settle\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
            Vagnus___024root___dump_triggers__stl(vlSelf);
#endif
            VL_FATAL_MT("../../../src/minimig-aga/agnus.v", 54, "", "Settle region did not converge.");
        }
        __VstlIterCount = ((IData)(1U) + __VstlIterCount);
        __VstlContinue = 0U;
        if (Vagnus___024root___eval_phase__stl(vlSelf)) {
            __VstlContinue = 1U;
        }
        vlSelfRef.__VstlFirstIteration = 0U;
    }
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus___024root___dump_triggers__stl(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___dump_triggers__stl\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VstlTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 0 is active: Internal 'stl' trigger - first iteration\n");
    }
    if ((2ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        VL_DBG_MSGF("         'stl' region trigger index 1 is active: @([hybrid] agnus.bl1.bltfl1.carry)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vagnus___024root___stl_sequent__TOP__0(Vagnus___024root* vlSelf);
void Vagnus___024root___act_sequent__TOP__0(Vagnus___024root* vlSelf);

VL_ATTR_COLD void Vagnus___024root___eval_stl(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_stl\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vagnus___024root___stl_sequent__TOP__0(vlSelf);
    }
    if ((3ULL & vlSelfRef.__VstlTriggered.word(0U))) {
        Vagnus___024root___act_sequent__TOP__0(vlSelf);
    }
}

extern const VlUnpacked<CData/*7:0*/, 4> Vagnus__ConstPool__TABLE_h78219485_0;
extern const VlUnpacked<IData/*17:0*/, 32> Vagnus__ConstPool__TABLE_hbbf19d2e_0;
extern const VlUnpacked<CData/*1:0*/, 512> Vagnus__ConstPool__TABLE_h4848e397_0;
extern const VlUnpacked<CData/*0:0*/, 1024> Vagnus__ConstPool__TABLE_h3d6e5953_0;
extern const VlUnpacked<CData/*7:0*/, 4> Vagnus__ConstPool__TABLE_h2dee94ce_0;
extern const VlUnpacked<CData/*7:0*/, 32> Vagnus__ConstPool__TABLE_h7cec6eda_0;

VL_ATTR_COLD void Vagnus___024root___stl_sequent__TOP__0(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___stl_sequent__TOP__0\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ agnus__DOT__req_spr;
    agnus__DOT__req_spr = 0;
    CData/*0:0*/ agnus__DOT__dma_spr;
    agnus__DOT__dma_spr = 0;
    CData/*0:0*/ agnus__DOT__dma_cop;
    agnus__DOT__dma_cop = 0;
    CData/*0:0*/ agnus__DOT__dma_blt;
    agnus__DOT__dma_blt = 0;
    CData/*0:0*/ agnus__DOT__dma_ref;
    agnus__DOT__dma_ref = 0;
    CData/*0:0*/ agnus__DOT__ack_cop;
    agnus__DOT__ack_cop = 0;
    CData/*0:0*/ agnus__DOT__ack_blt;
    agnus__DOT__ack_blt = 0;
    CData/*7:0*/ agnus__DOT__reg_address_aud;
    agnus__DOT__reg_address_aud = 0;
    IData/*19:0*/ agnus__DOT__address_bpl;
    agnus__DOT__address_bpl = 0;
    CData/*7:0*/ agnus__DOT__reg_address_bpl;
    agnus__DOT__reg_address_bpl = 0;
    CData/*7:0*/ agnus__DOT__reg_address_spr;
    agnus__DOT__reg_address_spr = 0;
    IData/*19:0*/ agnus__DOT__address_blt;
    agnus__DOT__address_blt = 0;
    CData/*0:0*/ agnus__DOT__we_blt;
    agnus__DOT__we_blt = 0;
    CData/*0:0*/ agnus__DOT__dsk1__DOT__dmaslot;
    agnus__DOT__dsk1__DOT__dmaslot = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT__shres;
    agnus__DOT__bpd1__DOT__shres = 0;
    CData/*4:0*/ agnus__DOT__bpd1__DOT__plane;
    agnus__DOT__bpd1__DOT__plane = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11 = 0;
    SData/*14:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16 = 0;
    CData/*0:0*/ agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18;
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18 = 0;
    CData/*0:0*/ agnus__DOT__cp1__DOT__illegalreg;
    agnus__DOT__cp1__DOT__illegalreg = 0;
    CData/*0:0*/ agnus__DOT__cp1__DOT__dma_req;
    agnus__DOT__cp1__DOT__dma_req = 0;
    CData/*0:0*/ agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0;
    agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0 = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT__lineinc;
    agnus__DOT__bl1__DOT__lineinc = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT__linedec;
    agnus__DOT__bl1__DOT__linedec = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT__lineadd;
    agnus__DOT__bl1__DOT__lineadd = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT__linesub;
    agnus__DOT__bl1__DOT__linesub = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0;
    agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0 = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0;
    agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0 = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13;
    agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13 = 0;
    CData/*0:0*/ agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14;
    agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14 = 0;
    IData/*17:0*/ agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot;
    agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot = 0;
    QData/*35:0*/ agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new;
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new = 0;
    QData/*35:0*/ agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old;
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old = 0;
    IData/*17:0*/ agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot;
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot = 0;
    IData/*19:0*/ agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr;
    agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr = 0;
    SData/*10:0*/ agnus__DOT__bc1__DOT__vbstop;
    agnus__DOT__bc1__DOT__vbstop = 0;
    CData/*0:0*/ agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_0;
    agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_0 = 0;
    CData/*0:0*/ agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_1;
    agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_1 = 0;
    SData/*9:0*/ __Vtableidx1;
    __Vtableidx1 = 0;
    SData/*8:0*/ __Vtableidx2;
    __Vtableidx2 = 0;
    CData/*1:0*/ __Vtableidx3;
    __Vtableidx3 = 0;
    CData/*4:0*/ __Vtableidx4;
    __Vtableidx4 = 0;
    CData/*1:0*/ __Vtableidx5;
    __Vtableidx5 = 0;
    CData/*4:0*/ __Vtableidx6;
    __Vtableidx6 = 0;
    CData/*4:0*/ __Vtableidx7;
    __Vtableidx7 = 0;
    // Body
    vlSelfRef.field1 = (1U & (~ (IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame)));
    vlSelfRef.sol = vlSelfRef.agnus__DOT__bc1__DOT__vpos_inc;
    vlSelfRef.agnus__DOT__bplen = (IData)((0x300U == 
                                           (0x300U 
                                            & (IData)(vlSelfRef.agnus__DOT__dmacon))));
    vlSelfRef._csync = (((IData)(vlSelfRef._hsync) 
                         & (IData)(vlSelfRef._vsync)) 
                        | (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vser));
    vlSelfRef.agnus__DOT__cp1__DOT__beam_match_skip 
        = ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__beam_match) 
           & ((~ (IData)(vlSelfRef.agnus__DOT__blit_busy)) 
              | ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2) 
                 >> 0xfU)));
    vlSelfRef.__VdfgRegularize_h3c5bdd75_0_2 = ((0xf8000U 
                                                 & ((IData)(vlSelfRef.data_in) 
                                                    << 0xfU)) 
                                                | (0x7fffU 
                                                   & ((IData)(vlSelfRef.data_in) 
                                                      >> 1U)));
    vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17 
        = ((3U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))
            ? 4U : ((0U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))
                     ? 1U : 2U));
    vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_0 
        = (1U & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                 [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                 >> 3U));
    vlSelfRef.agnus__DOT__bl1__DOT__first_word = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__width) 
                                                  == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt));
    vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_9 
        = ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfend) 
           & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
              >> 2U));
    vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_3 
        = ((0x100U & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                      [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                      << 7U)) | vlSelfRef.agnus__DOT__spr1__DOT__sprpos
           [vlSelfRef.agnus__DOT__spr1__DOT__sprsel]);
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_12 
        = ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt)) 
           & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__height_cnt)));
    vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_1 
        = (1U & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                 [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                 >> 2U));
    vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_4 
        = ((0x100U & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                      [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                      << 8U)) | (0xffU & (vlSelfRef.agnus__DOT__spr1__DOT__sprctl
                                          [vlSelfRef.agnus__DOT__spr1__DOT__sprsel] 
                                          >> 4U)));
    vlSelfRef.agnus__DOT__address_spr = ((vlSelfRef.agnus__DOT__spr1__DOT__sprpth
                                          [vlSelfRef.agnus__DOT__spr1__DOT__sprite] 
                                          << 0xfU) 
                                         | vlSelfRef.agnus__DOT__spr1__DOT__sprptl
                                         [vlSelfRef.agnus__DOT__spr1__DOT__sprite]);
    vlSelfRef.agnus__DOT__bl1__DOT__ife = (IData)((8U 
                                                   == 
                                                   (9U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))));
    vlSelfRef.agnus__DOT__bl1__DOT__efe = (IData)((0x10U 
                                                   == 
                                                   (0x11U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16 
        = ((1U & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__vdiwstrt) 
                  ^ (IData)(vlSelfRef.agnus__DOT__vpos)))
            ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod)
            : (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x8000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xfU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xfU))) 
                           & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xfU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                         << 0xfU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x4000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xeU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xeU))) 
                           & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xeU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                         << 0xeU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x2000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xdU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xdU))) 
                           & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xdU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                         << 0xdU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x1000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xcU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xcU))) 
                           & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xcU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                         << 0xcU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x800U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xbU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                               >> 0xbU))) 
                          & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xbU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                        << 0xbU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x400U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xaU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                               >> 0xaU))) 
                          & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xaU))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                        << 0xaU)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x200U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 9U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 9U))) 
                          & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 9U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                        << 9U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x100U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 8U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 8U))) 
                          & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 8U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                        << 8U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x80U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 7U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 7U))) 
                         & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 7U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                       << 7U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x40U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 6U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 6U))) 
                         & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 6U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                       << 6U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x20U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 5U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 5U))) 
                         & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 5U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                       << 5U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (0x10U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 4U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 4U))) 
                         & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 4U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                       << 4U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (8U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 3U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 3U))) & 
                      (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                          >> 3U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                    << 3U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (4U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 2U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 2U))) & 
                      (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                          >> 2U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                    << 2U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (2U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 1U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 1U))) & 
                      (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                          >> 1U))) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0)) 
                    << 1U)));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0)) 
           | (1U & ((((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold)) 
                      & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold))) 
                     & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat))) 
                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | ((IData)(((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                             >> 0xfU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 0xfU))) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                           >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                        >> 1U))) << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x4000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xeU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xeU))) 
                           << 0xeU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xdU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x2000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xdU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xdU))) 
                           << 0xdU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xcU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x1000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xcU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                                >> 0xcU))) 
                           << 0xcU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xbU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x800U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xbU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                               >> 0xbU))) 
                          << 0xbU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 0xaU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x400U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xaU)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                               >> 0xaU))) 
                          << 0xaU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x200U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 9U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 9U))) 
                          << 9U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x100U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 8U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                             >> 8U))) 
                          << 8U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x80U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 7U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 7U))) 
                         << 7U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x40U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 6U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 6U))) 
                         << 6U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x20U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 5U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 5U))) 
                         << 5U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (0x10U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 4U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                            >> 4U))) 
                         << 4U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (8U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 3U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 3U))) << 3U) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (4U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 2U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 2U))) << 2U) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (2U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 1U)) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 1U))) << 1U) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1)) 
           | (1U & ((((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold)) 
                      & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold))) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | ((IData)(((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                             >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                          >> 0xfU)) 
                        & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                              >> 0xfU))) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                            >> 2U))) 
              << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x4000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xeU)) << 0xeU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xeU)) << 0xeU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xcU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x2000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xdU)) << 0xdU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xdU)) << 0xdU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xbU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x1000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xcU)) << 0xcU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xcU)) << 0xcU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xaU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x800U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xbU)) << 0xbU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xbU)) << 0xbU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x400U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xaU)) << 0xaU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xaU)) << 0xaU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x200U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 9U)) << 9U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 9U)) << 9U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                   << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x100U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 8U)) << 8U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 8U)) << 8U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                   << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x80U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 7U)) << 7U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 7U)) << 7U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x40U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 6U)) << 6U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 6U)) << 6U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x20U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 5U)) << 5U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 5U)) << 5U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (0x10U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 4U)) << 4U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 4U)) << 4U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (8U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 3U)) << 3U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 3U)) << 3U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (4U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 2U)) << 2U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 2U)) << 2U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (2U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 1U)) << 1U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 1U)) << 1U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2)) 
           | (1U & ((((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold)) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat))) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | ((IData)(((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                             >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                          >> 0xfU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                           >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                        >> 3U))) << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x4000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xeU)) << 0xeU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xbU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x2000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xdU)) << 0xdU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xaU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x1000U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                                >> 0xcU)) << 0xcU) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x800U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xbU)) << 0xbU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x400U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 0xaU)) << 0xaU) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x200U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 9U)) << 9U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x100U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                               >> 8U)) << 8U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x80U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 7U)) << 7U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x40U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 6U)) << 6U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x20U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 5U)) << 5U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (0x10U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                              >> 4U)) << 4U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (8U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 3U)) << 3U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (4U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 2U)) << 2U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (2U & (((((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           >> 1U)) << 1U) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3)) 
           | (1U & ((((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold)) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | ((IData)((((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          >> 0xfU) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 0xfU))) 
                        & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                              >> 0xfU))) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                            >> 4U))) 
              << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x4000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xeU)) << 0xeU)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xeU)) << 0xeU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 0xaU))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x2000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xdU)) << 0xdU)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xdU)) << 0xdU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x1000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xcU)) << 0xcU)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xcU)) << 0xcU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x800U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 0xbU)) << 0xbU)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xbU)) << 0xbU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x400U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 0xaU)) << 0xaU)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xaU)) << 0xaU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x200U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 9U)) << 9U)) & 
                         ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                              >> 9U)) << 9U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                 << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x100U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 8U)) << 8U)) & 
                         ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                              >> 8U)) << 8U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                 << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x80U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 7U)) << 7U)) & (
                                                   (~ 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                     >> 7U)) 
                                                   << 7U)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x40U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 6U)) << 6U)) & (
                                                   (~ 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                     >> 6U)) 
                                                   << 6U)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x20U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 5U)) << 5U)) & (
                                                   (~ 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                     >> 5U)) 
                                                   << 5U)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (0x10U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 4U)) << 4U)) & (
                                                   (~ 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                     >> 4U)) 
                                                   << 4U)) 
                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (8U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 3U)) << 3U)) & ((~ 
                                                 ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                  >> 3U)) 
                                                << 3U)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (4U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 2U)) << 2U)) & ((~ 
                                                 ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                  >> 2U)) 
                                                << 2U)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (2U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 1U)) << 1U)) & ((~ 
                                                 ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                                  >> 1U)) 
                                                << 1U)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4)) 
           | (1U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold))) 
                     & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat))) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | ((IData)((((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          >> 0xfU) & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                         >> 0xfU))) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                           >> 0xfU)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                        >> 5U))) << 0xfU));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x4000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xeU)) << 0xeU)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x2000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xdU)) << 0xdU)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x1000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                  >> 0xcU)) << 0xcU)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x800U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 0xbU)) << 0xbU)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x400U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 0xaU)) << 0xaU)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x200U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 9U)) << 9U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x100U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                 >> 8U)) << 8U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x80U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 7U)) << 7U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x40U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 6U)) << 6U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x20U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 5U)) << 5U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (0x10U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                                >> 4U)) << 4U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (8U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 3U)) << 3U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (4U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 2U)) << 2U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (2U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold) 
                             >> 1U)) << 1U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5)) 
           | (1U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold))) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x8000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xfU)) << 0xfU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 9U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x4000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xeU)) << 0xeU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x2000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xdU)) << 0xdU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x1000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                 >> 0xcU)) << 0xcU)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x800U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xbU)) << 0xbU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x400U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 0xaU)) << 0xaU)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x200U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 9U)) << 9U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                   << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x100U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                                >> 8U)) << 8U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                   << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x80U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 7U)) << 7U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x40U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 6U)) << 6U)) & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x20U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 5U)) << 5U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (0x10U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                               >> 4U)) << 4U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                  >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (8U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 3U)) << 3U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (4U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 2U)) << 2U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (2U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat) 
                            >> 1U)) << 1U)) & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6)) 
           | (1U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat))) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0x7fffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x8000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 8U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xbfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x4000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 7U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xdfffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x2000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xefffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x1000U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                         & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                            << 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xf7ffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x800U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfbffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x400U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfdffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x200U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfeffU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x100U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                        & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                           << 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xff7fU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x80U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xffbfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x40U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          >> 1U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xffdfU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x20U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          >> 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xffefU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (0x10U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                          >> 3U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfff7U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (8U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfffbU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (4U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 5U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfffdU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (2U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 6U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7)) 
           | (1U & ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltahold) 
                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbhold)) 
                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcdat)) 
                    & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                       >> 7U))));
    agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14 
        = (1U & ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                     >> 3U)) & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__sign_del))));
    agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13 
        = (1U & ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__sign_del)) 
                 & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                    >> 3U)));
    vlSelfRef.agnus__DOT__bl1__DOT__desc = (IData)(
                                                   (2U 
                                                    == 
                                                    (3U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))));
    vlSelfRef.agnus__DOT__bl1__DOT__used = (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                             >> 8U) 
                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enad));
    vlSelfRef.agnus__DOT__blten = (IData)((0x240U == 
                                           (0x240U 
                                            & (IData)(vlSelfRef.agnus__DOT__dmacon))));
    agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0 
        = ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__bus_ena) 
           & (IData)(vlSelfRef.cck));
    vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14 
        = ((4U & ((~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                  << 2U)) | ((2U & ((~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                        >> 1U)) << 1U)) 
                             | (1U & (~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                         >> 2U)))));
    agnus__DOT__cp1__DOT__illegalreg = (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__cdang)) 
                                         & (0U == (0x180U 
                                                   & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)))) 
                                        | ((~ (IData)(vlSelfRef.ecs)) 
                                           & (0U == 
                                              (0x1c0U 
                                               & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)))));
    vlSelfRef.varbeamen = (1U & ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
                                 >> 7U));
    vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12 = (
                                                   (1U 
                                                    == 
                                                    (3U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))) 
                                                   | (2U 
                                                      == 
                                                      (3U 
                                                       & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))));
    vlSelfRef.agnus__DOT__hpos = (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r) 
                                   << 1U) | (IData)(vlSelfRef.cck));
    agnus__DOT__bpd1__DOT__shres = ((IData)(vlSelfRef.ecs) 
                                    & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
                                       >> 5U));
    vlSelfRef.agnus__DOT__bl1__DOT__bltamask = (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_word) 
                                                 & (1U 
                                                    == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt)))
                                                 ? 
                                                ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltafwm) 
                                                 & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltalwm))
                                                 : 
                                                ((1U 
                                                  == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt))
                                                  ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltalwm)
                                                  : 
                                                 ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_word)
                                                   ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltafwm)
                                                   : 0xffffU)));
    vlSelfRef.agnus__DOT__spr1__DOT__vstop = (((IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_1) 
                                               << 9U) 
                                              | (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_4));
    vlSelfRef.agnus__DOT__bl1__DOT__modsel = ((0x10U 
                                               & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                               ? ((8U 
                                                   & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                   ? 
                                                  ((4U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                    ? 0U
                                                    : 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                     ? 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                      ? 0U
                                                      : 3U)
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                      ? 
                                                     ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__sign)
                                                       ? 1U
                                                       : 2U)
                                                      : 
                                                     ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel)
                                                       ? 3U
                                                       : 0U))))
                                                   : 0U)
                                               : ((8U 
                                                   & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                   ? 
                                                  ((4U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                    ? 0U
                                                    : 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                     ? 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                      ? 1U
                                                      : 0U)
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                      ? 2U
                                                      : 3U)))
                                                   : 
                                                  ((4U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                    ? 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                     ? 0U
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                      ? 0U
                                                      : 3U))
                                                    : 0U)));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT____VdfgRegularize_h62043dcc_0_0 
        = ((0xf8000U & ((- (IData)((1U & (vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod
                                          [vlSelfRef.agnus__DOT__bl1__DOT__modsel] 
                                          >> 0xeU)))) 
                        << 0xfU)) | vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod
           [vlSelfRef.agnus__DOT__bl1__DOT__modsel]);
    vlSelfRef.agnus__DOT__spr1__DOT__newptr = (0xfffffU 
                                               & (vlSelfRef.agnus__DOT__address_spr 
                                                  + 
                                                  ((0U 
                                                    == 
                                                    (3U 
                                                     & ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__fmode) 
                                                        >> 2U)))
                                                    ? 1U
                                                    : 
                                                   ((3U 
                                                     == 
                                                     (3U 
                                                      & ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__fmode) 
                                                         >> 2U)))
                                                     ? 4U
                                                     : 2U))));
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_11 
        = ((1U & (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                   >> 9U) | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__ife) 
                             | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__efe))))
            ? 0xaU : 8U);
    if ((0x4000U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))) {
        vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan 
            = agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16;
        vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan 
            = agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_16;
    } else {
        vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan 
            = vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod;
        vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan 
            = vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod;
    }
    vlSelfRef.agnus__DOT__bl1__DOT__minterm_out = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt0) 
                                                   | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt1) 
                                                      | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt2) 
                                                         | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt3) 
                                                            | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt4) 
                                                               | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt5) 
                                                                  | ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt6) 
                                                                     | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltmt1__DOT__mt7))))))));
    agnus__DOT__bl1__DOT__lineadd = (1U & ((IData)(
                                                   (0U 
                                                    == 
                                                    (0x14U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                                           | (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                                               >> 4U) 
                                              & (IData)(agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14))));
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_7 
        = (1U & ((IData)((0x10U == (0x14U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                 | ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                        >> 4U)) & (IData)(agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_14))));
    agnus__DOT__bl1__DOT__linesub = (1U & ((IData)(
                                                   (4U 
                                                    == 
                                                    (0x14U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                                           | (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                                               >> 4U) 
                                              & (IData)(agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13))));
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_8 
        = (1U & ((IData)((0x14U == (0x14U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                 | ((~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                        >> 4U)) & (IData)(agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_13))));
    if ((0x10U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        if ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            if ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
            } else if ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                    vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
                    vlSelfRef.agnus__DOT__bl1__DOT__chsel = 0U;
                } else {
                    vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 3U;
                    vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
                }
            } else if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 2U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
            } else {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel 
                    = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel)
                        ? 3U : 0U);
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 3U;
            }
        } else {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
        }
    } else if ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        if ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 
                ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 2U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? 2U : 0U));
        } else if ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 1U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 1U;
            } else {
                vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
                vlSelfRef.agnus__DOT__bl1__DOT__chsel = 0U;
            }
        } else if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 2U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
        } else {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 3U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 3U;
        }
    } else if ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        if ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
        } else if ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 2U;
        } else {
            vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 3U;
            vlSelfRef.agnus__DOT__bl1__DOT__chsel = 3U;
        }
    } else {
        vlSelfRef.agnus__DOT__bl1__DOT__ptrsel = 0U;
        vlSelfRef.agnus__DOT__bl1__DOT__chsel = ((2U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                  ? 2U
                                                  : 0U);
    }
    __Vtableidx5 = vlSelfRef.agnus__DOT__bl1__DOT__chsel;
    vlSelfRef.agnus__DOT__reg_address_blt = Vagnus__ConstPool__TABLE_h78219485_0
        [__Vtableidx5];
    __Vtableidx6 = ((0x1eU & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                              >> 0xbU)) | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc));
    agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot 
        = Vagnus__ConstPool__TABLE_hbbf19d2e_0[__Vtableidx6];
    agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0 
        = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc) 
           & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt)));
    agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0 
        = ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)) 
           & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt)));
    __Vtableidx7 = ((0x1eU & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                              >> 0xbU)) | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc));
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot 
        = Vagnus__ConstPool__TABLE_hbbf19d2e_0[__Vtableidx7];
    vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_9 
        = (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__used)) 
            & (0xaU == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
           | (8U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)));
    vlSelfRef.agnus__DOT__cp1__DOT__enable = ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__bus_blk)) 
                                              & (IData)(agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0));
    vlSelfRef.agnus__DOT__cp1__DOT__selreg = ((1U & 
                                               ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                >> 2U)) 
                                              && ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                       >> 1U))) 
                                                  && ((1U 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)) 
                                                      && ((1U 
                                                           & (~ 
                                                              ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                               & (IData)(agnus__DOT__cp1__DOT__illegalreg)))) 
                                                          && ((1U 
                                                               & (~ 
                                                                  ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                                   & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag)))) 
                                                              && (1U 
                                                                  & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))))))));
    vlSelfRef.agnus__DOT__cp1__DOT__selins = ((1U & 
                                               ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                >> 2U)) 
                                              && ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                       >> 1U))) 
                                                  && ((1U 
                                                       & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))) 
                                                      || ((1U 
                                                           & (~ 
                                                              ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                               & (IData)(agnus__DOT__cp1__DOT__illegalreg)))) 
                                                          && (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                               & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag)) 
                                                              || (1U 
                                                                  & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)))))));
    if (vlSelfRef.varbeamen) {
        vlSelfRef.htotal = vlSelfRef.agnus__DOT__bc1__DOT__htotal_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__hbstrt = vlSelfRef.agnus__DOT__bc1__DOT__hbstrt_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__hbstop = vlSelfRef.agnus__DOT__bc1__DOT__hbstop_reg;
    } else {
        vlSelfRef.htotal = 0x1c4U;
        vlSelfRef.agnus__DOT__bc1__DOT__hbstrt = 0x19U;
        vlSelfRef.agnus__DOT__bc1__DOT__hbstop = 0x66U;
    }
    vlSelfRef.harddis = (1U & (IData)(((0U != (0x5000U 
                                               & (IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg))) 
                                       | (IData)(vlSelfRef.varbeamen))));
    agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_1 
        = (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
            >> 9U) & (IData)(vlSelfRef.varbeamen));
    vlSelfRef.agnus__DOT__bc1__DOT__vpos_equ_vtotal 
        = ((IData)(vlSelfRef.agnus__DOT__vpos) == ((IData)(vlSelfRef.varbeamen)
                                                    ? (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vtotal_reg)
                                                    : 
                                                   ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__pal)
                                                     ? 0x137U
                                                     : 0x105U)));
    agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_0 
        = (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
            >> 8U) & (IData)(vlSelfRef.varbeamen));
    agnus__DOT__bc1__DOT__vbstop = ((((IData)(vlSelfRef.agnus__DOT__bc1__DOT__beamcon0_reg) 
                                      >> 0xcU) & (IData)(vlSelfRef.varbeamen))
                                     ? (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vbstop_reg)
                                     : ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__pal)
                                         ? 0x19U : 0x14U));
    if ((0x25U == (IData)(vlSelfRef.agnus__DOT__hpos))) {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & (IData)(vlSelfRef.audio_dmas));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & (IData)(vlSelfRef.audio_dmal));
    } else if ((0x29U == (IData)(vlSelfRef.agnus__DOT__hpos))) {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmas) 
                                                    >> 1U));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmal) 
                                                    >> 1U));
    } else if ((0x2dU == (IData)(vlSelfRef.agnus__DOT__hpos))) {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmas) 
                                                    >> 2U));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & ((IData)(vlSelfRef.audio_dmal) 
                                                    >> 2U));
    } else {
        vlSelfRef.agnus__DOT__aud1__DOT__dmas = (1U 
                                                 & ((0x31U 
                                                     == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                                                    & ((IData)(vlSelfRef.audio_dmas) 
                                                       >> 3U)));
        vlSelfRef.agnus__DOT__aud1__DOT__dmal = (1U 
                                                 & ((0x31U 
                                                     == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                                                    & ((IData)(vlSelfRef.audio_dmal) 
                                                       >> 3U)));
    }
    vlSelfRef.strhor_denise = ((0xbU == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                               & ((8U < (IData)(vlSelfRef.agnus__DOT__vpos)) 
                                  | (IData)(vlSelfRef.ecs)));
    __Vtableidx2 = vlSelfRef.agnus__DOT__hpos;
    vlSelfRef.agnus__DOT__aud1__DOT__channel = Vagnus__ConstPool__TABLE_h4848e397_0
        [__Vtableidx2];
    vlSelfRef.strhor_paula = (0xdU == (IData)(vlSelfRef.agnus__DOT__hpos));
    __Vtableidx1 = (((IData)(vlSelfRef.floppy_speed) 
                     << 9U) | (IData)(vlSelfRef.agnus__DOT__hpos));
    agnus__DOT__dsk1__DOT__dmaslot = Vagnus__ConstPool__TABLE_h3d6e5953_0
        [__Vtableidx1];
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18 
        = ((~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
               >> 4U)) & ((~ (IData)(agnus__DOT__bpd1__DOT__shres)) 
                          & (3U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7 
        = ((IData)(agnus__DOT__bpd1__DOT__shres) & 
           (0U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11 
        = ((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
             >> 4U) & (0U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))) 
           | ((IData)(agnus__DOT__bpd1__DOT__shres) 
              & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12)));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5 
        = (1U & ((~ (IData)(agnus__DOT__bpd1__DOT__shres)) 
                 & (~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
                       >> 4U))));
    vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry 
        = ((0xfffeU & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltfl1__DOT__carry)) 
           | (1U & (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_word_del)
                      ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1) 
                         >> 2U) : (IData)(vlSelfRef.agnus__DOT__bl1__DOT__fcy)) 
                    ^ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__minterm_out))));
    agnus__DOT__bl1__DOT__lineinc = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_7) 
                                     & (0xf000U == 
                                        (0xf000U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    agnus__DOT__bl1__DOT__linedec = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_8) 
                                     & (0U == (0xf000U 
                                               & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))));
    vlSelfRef.agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_old 
        = (0xfffffffffULL & ((QData)((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltaold)) 
                             * (QData)((IData)(agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot))));
    vlSelfRef.agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_new 
        = (0xfffffffffULL & ((QData)((IData)(((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltadat) 
                                              & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltamask)))) 
                             * (QData)((IData)(agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shift_onehot))));
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new 
        = (0xfffffffffULL & ((QData)((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbdat)) 
                             * (QData)((IData)(agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot))));
    agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old 
        = (0xfffffffffULL & ((QData)((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltbold)) 
                             * (QData)((IData)(agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shift_onehot))));
    vlSelfRef.agnus__DOT__bl1__DOT__next_word = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_9) 
                                                 | ((0x1bU 
                                                     == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                                    | (0x18U 
                                                       == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))));
    agnus__DOT__cp1__DOT__dma_req = ((4U & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                      ? ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                   >> 1U))) 
                                         && ((1U & 
                                              (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))) 
                                             || (1U 
                                                 & (~ 
                                                    ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                     & (IData)(agnus__DOT__cp1__DOT__illegalreg))))))
                                      : ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                   >> 1U))) 
                                         && (1U & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state)))));
    agnus__DOT__dma_cop = (((IData)(agnus__DOT__cp1__DOT__dma_req) 
                            & (IData)(agnus__DOT__cp1__DOT____VdfgRegularize_he23179ad_0_0)) 
                           & (IData)((0x280U == (0x280U 
                                                 & (IData)(vlSelfRef.agnus__DOT__dmacon)))));
    if (agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_1) {
        vlSelfRef.agnus__DOT__bc1__DOT__vsstrt = vlSelfRef.agnus__DOT__bc1__DOT__vsstrt_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__vsstop = vlSelfRef.agnus__DOT__bc1__DOT__vsstop_reg;
    } else {
        vlSelfRef.agnus__DOT__bc1__DOT__vsstrt = 2U;
        vlSelfRef.agnus__DOT__bc1__DOT__vsstop = 5U;
    }
    vlSelfRef.agnus__DOT__bc1__DOT__last_line = ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame)
                                                  ? (IData)(vlSelfRef.agnus__DOT__bc1__DOT__extra_line)
                                                  : (IData)(vlSelfRef.agnus__DOT__bc1__DOT__vpos_equ_vtotal));
    if (agnus__DOT__bc1__DOT____VdfgRegularize_h90f6e462_0_0) {
        vlSelfRef.agnus__DOT__bc1__DOT__hsstrt = vlSelfRef.agnus__DOT__bc1__DOT__hsstrt_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__hsstop = vlSelfRef.agnus__DOT__bc1__DOT__hsstop_reg;
        vlSelfRef.agnus__DOT__bc1__DOT__hcenter = vlSelfRef.agnus__DOT__bc1__DOT__hcenter_reg;
    } else {
        vlSelfRef.agnus__DOT__bc1__DOT__hsstrt = 0x25U;
        vlSelfRef.agnus__DOT__bc1__DOT__hsstop = 0x46U;
        vlSelfRef.agnus__DOT__bc1__DOT__hcenter = 0x108U;
    }
    vlSelfRef.agnus__DOT__vblend = ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                    == (IData)(agnus__DOT__bc1__DOT__vbstop));
    vlSelfRef.agnus__DOT__vbl = ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                 <= (IData)(agnus__DOT__bc1__DOT__vbstop));
    vlSelfRef.agnus__DOT__aud1__DOT__audptout = vlSelfRef.agnus__DOT__aud1__DOT__audpt
        [vlSelfRef.agnus__DOT__aud1__DOT__channel];
    __Vtableidx3 = vlSelfRef.agnus__DOT__aud1__DOT__channel;
    agnus__DOT__reg_address_aud = Vagnus__ConstPool__TABLE_h2dee94ce_0
        [__Vtableidx3];
    agnus__DOT__dma_ref = ((9U == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                           | ((IData)(vlSelfRef.strhor_paula) 
                              | ((0x11U == (IData)(vlSelfRef.agnus__DOT__hpos)) 
                                 | (0x15U == (IData)(vlSelfRef.agnus__DOT__hpos)))));
    vlSelfRef.agnus__DOT__dma_dsk = ((IData)(vlSelfRef.disk_dmal) 
                                     & ((IData)(agnus__DOT__dsk1__DOT__dmaslot) 
                                        & (IData)(vlSelfRef.cck)));
    agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6 
        = (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
            & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12)) 
           | (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
               >> 4U) & (3U == (3U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))));
    vlSelfRef.agnus__DOT__bl1__DOT__shiftbout = (0xffffU 
                                                 & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)
                                                     ? 
                                                    ((IData)(agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new) 
                                                     | (IData)(
                                                               (agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old 
                                                                >> 0x10U)))
                                                     : 
                                                    ((IData)(
                                                             (agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_new 
                                                              >> 0x10U)) 
                                                     | (IData)(agnus__DOT__bl1__DOT__barrel_shifter_B__DOT__shifted_old))));
    vlSelfRef.sof = ((IData)(vlSelfRef.agnus__DOT__bc1__DOT__vpos_inc) 
                     & (IData)(vlSelfRef.agnus__DOT__bc1__DOT__last_line));
    vlSelfRef.agnus__DOT__spr1__DOT__dmastate_in = 
        ((~ ((IData)(vlSelfRef.agnus__DOT__vbl) | (
                                                   ((((IData)(vlSelfRef.ecs) 
                                                      & (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_1)) 
                                                     << 9U) 
                                                    | (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_4)) 
                                                   == 
                                                   (0x3ffU 
                                                    & (IData)(vlSelfRef.agnus__DOT__vpos))))) 
         & ((((((IData)(vlSelfRef.ecs) & (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_0)) 
               << 9U) | (IData)(vlSelfRef.agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_3)) 
             == (0x3ffU & (IData)(vlSelfRef.agnus__DOT__vpos))) 
            | vlSelfRef.agnus__DOT__spr1__DOT__dmastate_mem
            [vlSelfRef.agnus__DOT__spr1__DOT__sprsel]));
    agnus__DOT__reg_address_spr = (((IData)(vlSelfRef.agnus__DOT__spr1__DOT__enable) 
                                    & (1U == (3U & (IData)(vlSelfRef.agnus__DOT__hpos))))
                                    ? (((IData)(vlSelfRef.agnus__DOT__vblend) 
                                        | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprvstop) 
                                           & (~ (IData)(vlSelfRef.agnus__DOT__vbl))))
                                        ? ((4U & (IData)(vlSelfRef.agnus__DOT__hpos))
                                            ? (0xa0U 
                                               | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                  << 2U))
                                            : (0xa1U 
                                               | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                  << 2U)))
                                        : ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprdmastate)
                                            ? ((4U 
                                                & (IData)(vlSelfRef.agnus__DOT__hpos))
                                                ? (0xa2U 
                                                   | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                      << 2U))
                                                : (0xa3U 
                                                   | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite) 
                                                      << 2U)))
                                            : 0xffU))
                                    : 0xffU);
    agnus__DOT__req_spr = (((IData)(vlSelfRef.agnus__DOT__spr1__DOT__enable) 
                            & (1U == (3U & (IData)(vlSelfRef.agnus__DOT__hpos)))) 
                           && (((IData)(vlSelfRef.agnus__DOT__vblend) 
                                | ((IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprvstop) 
                                   & (~ (IData)(vlSelfRef.agnus__DOT__vbl)))) 
                               || (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprdmastate)));
    vlSelfRef.refresh = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                         && (IData)(agnus__DOT__dma_ref));
    vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq_match = 
        (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18) 
          & (7U == (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq))) 
         | (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6) 
             & (7U == (0xfU & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))) 
            | ((~ (IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_18)) 
               & ((~ (IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6)) 
                  & (7U == (7U & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))))));
    agnus__DOT__bpd1__DOT__plane = ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                     ? (1U & (~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)))
                                     : ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                         ? ((2U & (
                                                   (~ (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                                                   << 1U)) 
                                            | (1U & 
                                               (~ ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                                   >> 1U))))
                                         : ((((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_5) 
                                              & (0U 
                                                 == 
                                                 (3U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode)))) 
                                             | ((((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed) 
                                                  >> 4U) 
                                                 & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bp_fmode12)) 
                                                | ((IData)(agnus__DOT__bpd1__DOT__shres) 
                                                   & (3U 
                                                      == 
                                                      (3U 
                                                       & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__fmode))))))
                                             ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14)
                                             : ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_6)
                                                 ? 
                                                ((8U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                                                 | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14))
                                                 : 
                                                ((0x18U 
                                                  & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq)) 
                                                 | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14))))));
    agnus__DOT__dma_spr = ((IData)(agnus__DOT__req_spr) 
                           & (IData)((0x220U == (0x220U 
                                                 & (IData)(vlSelfRef.agnus__DOT__dmacon)))));
    agnus__DOT__address_bpl = ((vlSelfRef.agnus__DOT__bpd1__DOT__bplpth
                                [(7U & (IData)(agnus__DOT__bpd1__DOT__plane))] 
                                << 0xfU) | vlSelfRef.agnus__DOT__bpd1__DOT__bplptl
                               [(7U & (IData)(agnus__DOT__bpd1__DOT__plane))]);
    __Vtableidx4 = agnus__DOT__bpd1__DOT__plane;
    agnus__DOT__reg_address_bpl = Vagnus__ConstPool__TABLE_h7cec6eda_0
        [__Vtableidx4];
    vlSelfRef.agnus__DOT__dma_bpl = ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfrun) 
                                     & (((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__dmaena_delayed) 
                                         >> 1U) & ((IData)(vlSelfRef.cck) 
                                                   & ((IData)(agnus__DOT__bpd1__DOT__plane) 
                                                      < 
                                                      (0xfU 
                                                       & ((IData)(vlSelfRef.aga)
                                                           ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed)
                                                           : 
                                                          ((7U 
                                                            == 
                                                            (7U 
                                                             & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed)))
                                                            ? 4U
                                                            : 
                                                           (7U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bplcon0_delayed)))))))));
    vlSelfRef.agnus__DOT__bpd1__DOT__newpt = (0xfffffU 
                                              & (((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                                                   ? 
                                                  ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_9) 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfseq) 
                                                      >> 1U))
                                                   : 
                                                  ((IData)(agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                    ? (IData)(vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_9)
                                                    : (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__ddfend)))
                                                  ? 
                                                 ((1U 
                                                   & (IData)(agnus__DOT__bpd1__DOT__plane))
                                                   ? 
                                                  (agnus__DOT__address_bpl 
                                                   + 
                                                   (((0xf8000U 
                                                      & ((- (IData)(
                                                                    (1U 
                                                                     & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan) 
                                                                        >> 0xeU)))) 
                                                         << 0xfU)) 
                                                     | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl2mod_bscan)) 
                                                    + vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17))
                                                   : 
                                                  (agnus__DOT__address_bpl 
                                                   + 
                                                   (((0xf8000U 
                                                      & ((- (IData)(
                                                                    (1U 
                                                                     & ((IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan) 
                                                                        >> 0xeU)))) 
                                                         << 0xfU)) 
                                                     | (IData)(vlSelfRef.agnus__DOT__bpd1__DOT__bpl1mod_bscan)) 
                                                    + vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17)))
                                                  : 
                                                 (agnus__DOT__address_bpl 
                                                  + vlSelfRef.agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17)));
    vlSelfRef.agnus__DOT__ack_spr = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                                     && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                                         && ((1U & 
                                              (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                             && ((1U 
                                                  & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                                 && (IData)(agnus__DOT__dma_spr)))));
    agnus__DOT__ack_cop = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                           && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                               && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                   && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                       && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                           && (IData)(agnus__DOT__dma_cop))))));
    vlSelfRef.agnus__DOT__bl1__DOT__enable = ((IData)(vlSelfRef.cck) 
                                              & ((~ 
                                                  ((IData)(vlSelfRef.agnus__DOT__dma_dsk) 
                                                   | ((IData)(agnus__DOT__dma_ref) 
                                                      | ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal) 
                                                         | ((IData)(agnus__DOT__dma_spr) 
                                                            | ((IData)(vlSelfRef.agnus__DOT__dma_bpl) 
                                                               | ((IData)(agnus__DOT__dma_cop) 
                                                                  | (3U 
                                                                     == (IData)(vlSelfRef.agnus__DOT__bls_cnt))))))))) 
                                                 & (IData)(vlSelfRef.agnus__DOT__blten)));
    vlSelfRef.agnus__DOT__cp1__DOT__dma_ack = ((IData)(agnus__DOT__ack_cop) 
                                               & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__enable));
    vlSelfRef.agnus__DOT__cp1__DOT__copper_next = (
                                                   (4U 
                                                    & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                    ? 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                     ? 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                      ? 
                                                     ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                       ? 7U
                                                       : 6U)
                                                      : 
                                                     ((1U 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                                       ? 
                                                      ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                        ? 6U
                                                        : 4U)
                                                       : 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__beam_match_wait)
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                         ? 6U
                                                         : 4U)
                                                        : 6U)))
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                      ? 
                                                     (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                       & (IData)(agnus__DOT__cp1__DOT__illegalreg))
                                                       ? 5U
                                                       : 
                                                      (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                        & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag))
                                                        ? 
                                                       ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                         ? 4U
                                                         : 5U)
                                                        : 
                                                       ((1U 
                                                         & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2))
                                                         ? 
                                                        ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                          ? 7U
                                                          : 5U)
                                                         : 
                                                        ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                          ? 4U
                                                          : 5U))))
                                                      : 
                                                     ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                       ? 5U
                                                       : 4U)))
                                                    : 
                                                   ((2U 
                                                     & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                     ? 4U
                                                     : 
                                                    ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                      ? 4U
                                                      : 
                                                     ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)
                                                       ? 4U
                                                       : 0U))));
    vlSelfRef.agnus__DOT__bl1__DOT__decbsh = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
                                              & (0x18U 
                                                 == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)));
    vlSelfRef.int3 = ((((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_9) 
                        & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt))) 
                       | (0x18U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                      & ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__height_cnt)) 
                         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)));
    vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_dec = 
        ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable) 
         & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__next_word));
    agnus__DOT__dma_blt = ((((0x10U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                              ? ((1U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                        >> 3U)) && 
                                 ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                            >> 2U))) 
                                  && ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                       ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                          && (1U & 
                                              ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                               >> 9U)))
                                       : ((1U & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                          && (1U & 
                                              (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                >> 9U) 
                                               & ((IData)(
                                                          (0x12U 
                                                           != 
                                                           (0x12U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1)))) 
                                                  | (IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_pixel))))))))
                              : ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                  ? ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                               >> 2U))) 
                                     && (1U & ((2U 
                                                & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                ? (
                                                   (1U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                                   || (1U 
                                                       & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                          >> 9U)))
                                                : (
                                                   (1U 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                    ? 
                                                   ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0) 
                                                    >> 0xbU)
                                                    : 
                                                   ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__used) 
                                                    & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__pipeline_full))))))
                                  : ((1U & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                            >> 2U)) 
                                     && ((1U & (~ ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                   >> 1U))) 
                                         && (1U & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))))))) 
                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)) 
                           & (IData)(vlSelfRef.agnus__DOT__blten));
    vlSelfRef.agnus__DOT__cp1__DOT__skip = ((1U & ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state) 
                                                   >> 2U)) 
                                            && ((2U 
                                                 & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                 ? 
                                                ((1U 
                                                  & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))) 
                                                 && ((1U 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                     && (IData)(vlSelfRef.agnus__DOT__cp1__DOT__beam_match_skip)))
                                                 : 
                                                ((1U 
                                                  & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__copper_state))
                                                  ? 
                                                 ((1U 
                                                   & (~ 
                                                      ((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                       & (IData)(agnus__DOT__cp1__DOT__illegalreg)))) 
                                                  && (((~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2)) 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag)) 
                                                      && (1U 
                                                          & (~ (IData)(vlSelfRef.agnus__DOT__cp1__DOT__dma_ack)))))
                                                  : (IData)(vlSelfRef.agnus__DOT__cp1__DOT__skip_flag))));
    vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_rld = 
        (((IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt_dec) 
          & (1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__width_cnt))) 
         | ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)));
    vlSelfRef.dbr = ((IData)(vlSelfRef.agnus__DOT__dma_dsk) 
                     || ((IData)(agnus__DOT__dma_ref) 
                         || ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal) 
                             || ((IData)(vlSelfRef.agnus__DOT__dma_bpl) 
                                 || ((IData)(agnus__DOT__dma_spr) 
                                     || ((IData)(agnus__DOT__dma_cop) 
                                         || ((IData)(agnus__DOT__dma_blt) 
                                             & (3U 
                                                != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))))))));
    vlSelfRef.cpu_custom = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                            && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                                && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                    && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                        && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                            && ((1U 
                                                 & (~ (IData)(agnus__DOT__dma_cop))) 
                                                && (1U 
                                                    & (~ 
                                                       ((IData)(agnus__DOT__dma_blt) 
                                                        & (3U 
                                                           != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))))))))));
    agnus__DOT__ack_blt = ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_dsk))) 
                           && ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                               && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                   && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                       && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                           && ((1U 
                                                & (~ (IData)(agnus__DOT__dma_cop))) 
                                               && ((IData)(agnus__DOT__dma_blt) 
                                                   & (3U 
                                                      != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))))))));
    if ((0x10U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__submod = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel) 
                                                            & (IData)(agnus__DOT__bl1__DOT__linesub))
                                                            : (IData)(agnus__DOT__bl1__DOT__linesub))
                                                           : 
                                                          ((1U 
                                                            & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                           && (IData)(agnus__DOT__bl1__DOT__linesub)))));
        vlSelfRef.agnus__DOT__bl1__DOT__addmod = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel) 
                                                            & (IData)(agnus__DOT__bl1__DOT__lineadd))
                                                            : (IData)(agnus__DOT__bl1__DOT__lineadd))
                                                           : 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state)) 
                                                           || (IData)(agnus__DOT__bl1__DOT__lineadd)))));
        vlSelfRef.agnus__DOT__bl1__DOT__incptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel) 
                                                            & (IData)(agnus__DOT__bl1__DOT__lineinc))
                                                            : (IData)(agnus__DOT__bl1__DOT__lineinc))
                                                           : 
                                                          ((1U 
                                                            & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                           && (IData)(agnus__DOT__bl1__DOT__lineinc)))));
        vlSelfRef.agnus__DOT__bl1__DOT__decptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? 
                                                           ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel) 
                                                            & (IData)(agnus__DOT__bl1__DOT__linedec))
                                                            : (IData)(agnus__DOT__bl1__DOT__linedec))
                                                           : 
                                                          ((1U 
                                                            & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                           && (IData)(agnus__DOT__bl1__DOT__linedec)))));
        vlSelfRef.agnus__DOT__bl1__DOT__blt_next = 
            ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
              ? ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 0U : ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                               ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? 0x1aU : 0x1bU)
                               : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? 0x18U : 0x1aU))
                           : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                               ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? 0x1bU : 0x19U)
                               : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? ((1U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__height_cnt))
                                       ? 0U : 0x19U)
                                   : 0x18U)))) : 0U);
        vlSelfRef.agnus__DOT__bl1__DOT__enaptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 3U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 2U))) 
                                                      && ((2U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          ((1U 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                            ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                                            : 
                                                           ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__first_line_pixel)) 
                                                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)))
                                                           : (IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable))));
    } else if ((8U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) {
        vlSelfRef.agnus__DOT__bl1__DOT__submod = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && ((2U 
                                                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                       ? (IData)(agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0)
                                                       : 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                        ? (IData)(agnus__DOT__bl1__DOT____VdfgExtracted_h37f113f4__0)
                                                        : 
                                                       ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc) 
                                                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__last_word_del)))));
        vlSelfRef.agnus__DOT__bl1__DOT__addmod = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && ((2U 
                                                       & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                       ? (IData)(agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0)
                                                       : 
                                                      ((1U 
                                                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                        ? (IData)(agnus__DOT__bl1__DOT____VdfgExtracted_ha70b9624__0)
                                                        : 
                                                       ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)) 
                                                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__last_word_del)))));
        vlSelfRef.agnus__DOT__bl1__DOT__incptr = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && (1U 
                                                      & ((2U 
                                                          & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                          ? 
                                                         ((1U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))
                                                           : 
                                                          (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)))
                                                          : 
                                                         ((1U 
                                                           & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                           ? 
                                                          (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))
                                                           : 
                                                          (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))))));
        vlSelfRef.agnus__DOT__bl1__DOT__decptr = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc));
        vlSelfRef.agnus__DOT__bl1__DOT__blt_next = 
            ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
              ? ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 0U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? 0U : ((IData)(vlSelfRef.cck)
                                    ? 4U : 0xcU))) : 
             ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
               ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                   ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                       ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_11)
                       : 0xbU) : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                   ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__used)
                                       ? 8U : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_12)
                                                ? 0U
                                                : 9U))
                                   : 0xaU)) : ((1U 
                                                & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                                                ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                                    ? 
                                                   ((0x400U 
                                                     & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon0))
                                                     ? 0xbU
                                                     : (IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_11))
                                                    : 9U)
                                                : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                                    ? 
                                                   ((IData)(vlSelfRef.agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_12)
                                                     ? 
                                                    ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__used)
                                                      ? 0xcU
                                                      : 0U)
                                                     : 9U)
                                                    : 8U))));
        vlSelfRef.agnus__DOT__bl1__DOT__enaptr = ((1U 
                                                   & (~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                       >> 2U))) 
                                                  && (IData)(agnus__DOT__ack_blt));
    } else {
        vlSelfRef.agnus__DOT__bl1__DOT__submod = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc) 
                                                              & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__last_word_del)))));
        vlSelfRef.agnus__DOT__bl1__DOT__addmod = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc)) 
                                                              & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__last_word_del)))));
        vlSelfRef.agnus__DOT__bl1__DOT__incptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && (1U 
                                                              & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))))));
        vlSelfRef.agnus__DOT__bl1__DOT__decptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && (IData)(vlSelfRef.agnus__DOT__bl1__DOT__desc))));
        vlSelfRef.agnus__DOT__bl1__DOT__blt_next = 
            ((4U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
              ? ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                  ? 0U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                           ? 0U : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                                    ? 0U : 4U))) : 
             ((2U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
               ? 0U : ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))
                        ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                            ? ((1U & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltcon1))
                                ? 0x19U : 9U) : 1U)
                        : ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enable)
                            ? ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__start)
                                ? 1U : 0U) : 0U))));
        vlSelfRef.agnus__DOT__bl1__DOT__enaptr = ((1U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                      >> 2U)) 
                                                  && ((1U 
                                                       & (~ 
                                                          ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state) 
                                                           >> 1U))) 
                                                      && ((1U 
                                                           & (~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__blt_state))) 
                                                          && (IData)(agnus__DOT__ack_blt))));
    }
    agnus__DOT__we_blt = ((IData)(agnus__DOT__ack_blt) 
                          & (3U == (IData)(vlSelfRef.agnus__DOT__bl1__DOT__chsel)));
    if (vlSelfRef.agnus__DOT__dma_dsk) {
        vlSelfRef.agnus__DOT__dsk1__DOT__address_outnew 
            = (0xfffffU & ((IData)(1U) + vlSelfRef.agnus__DOT__address_dsk));
        vlSelfRef.agnus__DOT__reg_address = (0xffU 
                                             & ((IData)(vlSelfRef.disk_dmas)
                                                 ? 0x13U
                                                 : 4U));
    } else {
        vlSelfRef.agnus__DOT__dsk1__DOT__address_outnew 
            = (0xfffffU & vlSelfRef.__VdfgRegularize_h3c5bdd75_0_2);
        vlSelfRef.agnus__DOT__reg_address = (0xffU 
                                             & ((IData)(agnus__DOT__dma_ref)
                                                 ? 0xffU
                                                 : 
                                                ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal)
                                                  ? (IData)(agnus__DOT__reg_address_aud)
                                                  : 
                                                 ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                   ? (IData)(agnus__DOT__reg_address_bpl)
                                                   : 
                                                  ((IData)(agnus__DOT__dma_spr)
                                                    ? (IData)(agnus__DOT__reg_address_spr)
                                                    : 
                                                   ((IData)(agnus__DOT__dma_cop)
                                                     ? 
                                                    (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__enable) 
                                                      & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__selins))
                                                      ? 0x46U
                                                      : 
                                                     (((IData)(vlSelfRef.agnus__DOT__cp1__DOT__enable) 
                                                       & (IData)(vlSelfRef.agnus__DOT__cp1__DOT__selreg))
                                                       ? 
                                                      ((IData)(vlSelfRef.agnus__DOT__cp1__DOT__ir2) 
                                                       >> 1U)
                                                       : 0xffU))
                                                     : 
                                                    (((IData)(agnus__DOT__dma_blt) 
                                                      & (3U 
                                                         != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))
                                                      ? (IData)(vlSelfRef.agnus__DOT__reg_address_blt)
                                                      : 
                                                     (((IData)(vlSelfRef.aen) 
                                                       & ((IData)(vlSelfRef.rd) 
                                                          | ((IData)(vlSelfRef.hwr) 
                                                             | (IData)(vlSelfRef.lwr))))
                                                       ? (IData)(vlSelfRef.address_in)
                                                       : 0xffU))))))));
    }
    vlSelfRef.reg_address_out = vlSelfRef.agnus__DOT__reg_address;
    vlSelfRef.agnus__DOT__aud1__DOT__audlcena = (1U 
                                                 & (IData)(
                                                           ((0x40U 
                                                             == 
                                                             (0xc0U 
                                                              & (IData)(vlSelfRef.agnus__DOT__reg_address))) 
                                                            & ((~ (IData)(
                                                                          (0U 
                                                                           != 
                                                                           (6U 
                                                                            & (IData)(vlSelfRef.agnus__DOT__reg_address))))) 
                                                               & (((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                                   >> 5U) 
                                                                  ^ 
                                                                  ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                                   >> 4U))))));
    vlSelfRef.agnus__DOT__aud1__DOT__audlcsel = ((2U 
                                                  & ((~ 
                                                      ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                       >> 4U)) 
                                                     << 1U)) 
                                                 | (1U 
                                                    & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                       >> 3U)));
    vlSelfRef.agnus__DOT__bpd1__DOT__bplptr_sel = (7U 
                                                   & ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                                       ? (IData)(agnus__DOT__bpd1__DOT__plane)
                                                       : 
                                                      ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                       >> 1U)));
    if (vlSelfRef.agnus__DOT__ack_spr) {
        vlSelfRef.agnus__DOT__spr1__DOT__ptsel = (7U 
                                                  & (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite));
        vlSelfRef.agnus__DOT__spr1__DOT__pcsel = (7U 
                                                  & (IData)(vlSelfRef.agnus__DOT__spr1__DOT__sprite));
    } else {
        vlSelfRef.agnus__DOT__spr1__DOT__ptsel = (7U 
                                                  & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                     >> 1U));
        vlSelfRef.agnus__DOT__spr1__DOT__pcsel = (7U 
                                                  & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                                     >> 2U));
    }
    vlSelfRef.data_out = ((((2U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                            | (0x15U == (IData)(vlSelfRef.agnus__DOT__reg_address)))
                            ? (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_frame) 
                                << 0xfU) | (((IData)(vlSelfRef.ecs) 
                                             << 0xdU) 
                                            | (((IData)(vlSelfRef.ntsc) 
                                                << 0xcU) 
                                               | ((0x300U 
                                                   & ((- (IData)((IData)(vlSelfRef.aga))) 
                                                      << 8U)) 
                                                  | (((IData)(vlSelfRef.agnus__DOT__bc1__DOT__long_line) 
                                                      << 7U) 
                                                     | (7U 
                                                        & ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                                           >> 8U)))))))
                            : (((3U == (IData)(vlSelfRef.agnus__DOT__reg_address)) 
                                | (0x16U == (IData)(vlSelfRef.agnus__DOT__reg_address)))
                                ? ((0xff00U & ((IData)(vlSelfRef.agnus__DOT__vpos) 
                                               << 8U)) 
                                   | (IData)(vlSelfRef.agnus__DOT__bc1__DOT__hpos_r))
                                : 0U)) | (((1U == (IData)(vlSelfRef.agnus__DOT__reg_address))
                                            ? (((IData)(vlSelfRef.agnus__DOT__blit_busy) 
                                                << 0xeU) 
                                               | (((IData)(vlSelfRef.agnus__DOT__blit_zero) 
                                                   << 0xdU) 
                                                  | (IData)(vlSelfRef.agnus__DOT__dmacon)))
                                            : 0U) | 
                                          ((IData)(agnus__DOT__we_blt)
                                            ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__bltdhold)
                                            : 0U)));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel 
        = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enaptr)
            ? (IData)(vlSelfRef.agnus__DOT__bl1__DOT__ptrsel)
            : ((2U & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                      >> 2U)) | (1U & ((IData)(vlSelfRef.agnus__DOT__reg_address) 
                                       >> 1U))));
    agnus__DOT__address_blt = ((vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth
                                [vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel] 
                                << 0xfU) | vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl
                               [vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel]);
    if (vlSelfRef.agnus__DOT__dma_dsk) {
        vlSelfRef.dbwe = (1U & (~ (IData)(vlSelfRef.disk_dmas)));
        vlSelfRef.address_out = vlSelfRef.agnus__DOT__address_dsk;
    } else {
        vlSelfRef.dbwe = (1U & ((1U & (~ (IData)(agnus__DOT__dma_ref))) 
                                && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal))) 
                                    && ((1U & (~ (IData)(vlSelfRef.agnus__DOT__dma_bpl))) 
                                        && ((1U & (~ (IData)(agnus__DOT__dma_spr))) 
                                            && ((1U 
                                                 & (~ (IData)(agnus__DOT__dma_cop))) 
                                                && (((IData)(agnus__DOT__dma_blt) 
                                                     & (3U 
                                                        != (IData)(vlSelfRef.agnus__DOT__bls_cnt))) 
                                                    && (IData)(agnus__DOT__we_blt))))))));
        vlSelfRef.address_out = ((IData)(agnus__DOT__dma_ref)
                                  ? 0U : ((IData)(vlSelfRef.agnus__DOT__aud1__DOT__dmal)
                                           ? vlSelfRef.agnus__DOT__aud1__DOT__audpt
                                          [vlSelfRef.agnus__DOT__aud1__DOT__channel]
                                           : ((IData)(vlSelfRef.agnus__DOT__dma_bpl)
                                               ? agnus__DOT__address_bpl
                                               : ((IData)(agnus__DOT__dma_spr)
                                                   ? vlSelfRef.agnus__DOT__address_spr
                                                   : 
                                                  ((IData)(agnus__DOT__dma_cop)
                                                    ? vlSelfRef.agnus__DOT__address_cop
                                                    : 
                                                   (((IData)(agnus__DOT__dma_blt) 
                                                     & (3U 
                                                        != (IData)(vlSelfRef.agnus__DOT__bls_cnt)))
                                                     ? agnus__DOT__address_blt
                                                     : 0U))))));
    }
    agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr 
        = (0xfffffU & (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__decptr)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__incptr))
                        ? ((IData)(1U) + agnus__DOT__address_blt)
                        : (agnus__DOT__address_blt 
                           - ((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__incptr)) 
                              & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__decptr)))));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__newptr 
        = (0xfffffU & (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__submod)) 
                        & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__addmod))
                        ? (agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr 
                           + vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT____VdfgRegularize_h62043dcc_0_0)
                        : (((~ (IData)(vlSelfRef.agnus__DOT__bl1__DOT__addmod)) 
                            & (IData)(vlSelfRef.agnus__DOT__bl1__DOT__submod))
                            ? (agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr 
                               - vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT____VdfgRegularize_h62043dcc_0_0)
                            : agnus__DOT__bl1__DOT__address_generator_1__DOT__t_newptr)));
    vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_in 
        = ((IData)(vlSelfRef.agnus__DOT__bl1__DOT__enaptr)
            ? vlSelfRef.agnus__DOT__bl1__DOT__address_generator_1__DOT__newptr
            : vlSelfRef.__VdfgRegularize_h3c5bdd75_0_2);
}

VL_ATTR_COLD void Vagnus___024root___eval_triggers__stl(Vagnus___024root* vlSelf);

VL_ATTR_COLD bool Vagnus___024root___eval_phase__stl(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___eval_phase__stl\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    CData/*0:0*/ __VstlExecute;
    // Body
    Vagnus___024root___eval_triggers__stl(vlSelf);
    __VstlExecute = vlSelfRef.__VstlTriggered.any();
    if (__VstlExecute) {
        Vagnus___024root___eval_stl(vlSelf);
    }
    return (__VstlExecute);
}

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus___024root___dump_triggers__ico(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___dump_triggers__ico\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
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
VL_ATTR_COLD void Vagnus___024root___dump_triggers__act(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___dump_triggers__act\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VactTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 0 is active: @([hybrid] agnus.bl1.bltfl1.carry)\n");
    }
    if ((2ULL & vlSelfRef.__VactTriggered.word(0U))) {
        VL_DBG_MSGF("         'act' region trigger index 1 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

#ifdef VL_DEBUG
VL_ATTR_COLD void Vagnus___024root___dump_triggers__nba(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___dump_triggers__nba\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    if ((1U & (~ vlSelfRef.__VnbaTriggered.any()))) {
        VL_DBG_MSGF("         No triggers active\n");
    }
    if ((1ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 0 is active: @([hybrid] agnus.bl1.bltfl1.carry)\n");
    }
    if ((2ULL & vlSelfRef.__VnbaTriggered.word(0U))) {
        VL_DBG_MSGF("         'nba' region trigger index 1 is active: @(posedge clk)\n");
    }
}
#endif  // VL_DEBUG

VL_ATTR_COLD void Vagnus___024root___ctor_var_reset(Vagnus___024root* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus___024root___ctor_var_reset\n"); );
    Vagnus__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->clk7_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1674876599471182170ull);
    vlSelf->cck = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16163541962503348705ull);
    vlSelf->reset = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9928399931838511862ull);
    vlSelf->aen = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10273666400050559500ull);
    vlSelf->rd = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10303652599525973633ull);
    vlSelf->hwr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13625843337315403699ull);
    vlSelf->lwr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7232435216762762385ull);
    vlSelf->data_in = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10574596302020702150ull);
    vlSelf->data_out = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11675680895196038875ull);
    vlSelf->address_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 10876871843634673166ull);
    vlSelf->address_out = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 16520239430411259709ull);
    vlSelf->reg_address_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9386968039567798889ull);
    vlSelf->cpu_custom = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18151418632443228490ull);
    vlSelf->dbr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4074308482846119752ull);
    vlSelf->dbwe = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16445899479027896702ull);
    vlSelf->refresh = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12242286718717544838ull);
    vlSelf->_hsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11470752221461756747ull);
    vlSelf->_vsync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10924989827455727735ull);
    vlSelf->_csync = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9881154449261449394ull);
    vlSelf->field1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6996431917986141852ull);
    vlSelf->lace = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14091033307413749139ull);
    vlSelf->hblank = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6030576639516052719ull);
    vlSelf->vblank = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18047747227580282234ull);
    vlSelf->hde = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12882197036088020736ull);
    vlSelf->sol = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 739257594701741756ull);
    vlSelf->sof = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7496247244052230992ull);
    vlSelf->vbl_int = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10064151831238951005ull);
    vlSelf->strhor_denise = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10781768859317474396ull);
    vlSelf->strhor_paula = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1556071011009735583ull);
    vlSelf->htotal = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 997051761989842679ull);
    vlSelf->harddis = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15113905107406558520ull);
    vlSelf->varbeamen = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3223898635922026186ull);
    vlSelf->int3 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14453772997496377891ull);
    vlSelf->audio_dmal = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 13618173280352938351ull);
    vlSelf->audio_dmas = VL_SCOPED_RAND_RESET_I(4, __VscopeHash, 5976784137687008699ull);
    vlSelf->disk_dmal = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14969447001970336043ull);
    vlSelf->disk_dmas = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4355606309660810439ull);
    vlSelf->bls = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14022721773301135254ull);
    vlSelf->ntsc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2976193588104925908ull);
    vlSelf->a1k = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17705640770547906661ull);
    vlSelf->ecs = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15277934814765900103ull);
    vlSelf->aga = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5205082699332066466ull);
    vlSelf->floppy_speed = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14384700425780437133ull);
    vlSelf->agnus__DOT__blten = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10195770221186174713ull);
    vlSelf->agnus__DOT__ack_spr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14959319804748616168ull);
    vlSelf->agnus__DOT__reg_address = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 8528086954815741125ull);
    vlSelf->agnus__DOT__dma_dsk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13299270544715536898ull);
    vlSelf->agnus__DOT__address_dsk = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 17475426539423841943ull);
    vlSelf->agnus__DOT__dma_bpl = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6050729295346273302ull);
    vlSelf->agnus__DOT__address_spr = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 6901026219890809038ull);
    vlSelf->agnus__DOT__address_cop = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 1865913679584753424ull);
    vlSelf->agnus__DOT__blit_zero = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12567132735585782386ull);
    vlSelf->agnus__DOT__reg_address_blt = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 3724275250996530193ull);
    vlSelf->agnus__DOT__bls_cnt = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13050678827955400614ull);
    vlSelf->agnus__DOT__dmacon = VL_SCOPED_RAND_RESET_I(13, __VscopeHash, 10312842272322185283ull);
    vlSelf->agnus__DOT__blit_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1082582746364339424ull);
    vlSelf->agnus__DOT__bplen = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7725324958519209138ull);
    vlSelf->agnus__DOT__hpos = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 15462447702344187366ull);
    vlSelf->agnus__DOT__vpos = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 11989163840597762376ull);
    vlSelf->agnus__DOT__vbl = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18052838895848941783ull);
    vlSelf->agnus__DOT__vblend = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7154961018515027834ull);
    vlSelf->agnus__DOT__dsk1__DOT__address_outnew = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 8186521779191370061ull);
    vlSelf->agnus__DOT__aud1__DOT__audlcena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 86575561848850015ull);
    vlSelf->agnus__DOT__aud1__DOT__audlcsel = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 9010306163472937008ull);
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->agnus__DOT__aud1__DOT__audlch[__Vi0] = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 7971721199231375574ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->agnus__DOT__aud1__DOT__audlcl[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 1214227082434362661ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->agnus__DOT__aud1__DOT__audpt[__Vi0] = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 6172011823560909414ull);
    }
    vlSelf->agnus__DOT__aud1__DOT__audptout = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 12292699637224298362ull);
    vlSelf->agnus__DOT__aud1__DOT__channel = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 1941140811332722049ull);
    vlSelf->agnus__DOT__aud1__DOT__dmal = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7972390494849445283ull);
    vlSelf->agnus__DOT__aud1__DOT__dmas = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18042088988234749302ull);
    vlSelf->agnus__DOT__bpd1__DOT__ddfstrt = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 12918513839404135789ull);
    vlSelf->agnus__DOT__bpd1__DOT__ddfstop = VL_SCOPED_RAND_RESET_I(7, __VscopeHash, 1270940805575920331ull);
    vlSelf->agnus__DOT__bpd1__DOT__bpl1mod = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 8587539695775706728ull);
    vlSelf->agnus__DOT__bpd1__DOT__bpl2mod = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 13855750953473093267ull);
    vlSelf->agnus__DOT__bpd1__DOT__bpl1mod_bscan = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 1253846011779396417ull);
    vlSelf->agnus__DOT__bpd1__DOT__bpl2mod_bscan = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 10993194738675712126ull);
    vlSelf->agnus__DOT__bpd1__DOT__bplcon0 = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 2532261370823375526ull);
    vlSelf->agnus__DOT__bpd1__DOT__bplcon0_delayed = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 5868532899131761837ull);
    for (int __Vi0 = 0; __Vi0 < 2; ++__Vi0) {
        vlSelf->agnus__DOT__bpd1__DOT__bplcon0_delay[__Vi0] = VL_SCOPED_RAND_RESET_I(6, __VscopeHash, 17660756545734928079ull);
    }
    vlSelf->agnus__DOT__bpd1__DOT__fmode = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8786153026754100864ull);
    vlSelf->agnus__DOT__bpd1__DOT__newpt = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 2256653355603596223ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus__DOT__bpd1__DOT__bplpth[__Vi0] = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 17848540687147697251ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus__DOT__bpd1__DOT__bplptl[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 11627200727122864673ull);
    }
    vlSelf->agnus__DOT__bpd1__DOT__hardena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14558180229623446230ull);
    vlSelf->agnus__DOT__bpd1__DOT__softena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16568844226151991673ull);
    vlSelf->agnus__DOT__bpd1__DOT__ddfena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7021507564766198721ull);
    vlSelf->agnus__DOT__bpd1__DOT__ddfena_0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8027820748127697465ull);
    vlSelf->agnus__DOT__bpd1__DOT__ddfseq = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 8358130891065660748ull);
    vlSelf->agnus__DOT__bpd1__DOT__ddfrun = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13954436411725897313ull);
    vlSelf->agnus__DOT__bpd1__DOT__ddfend = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14608140032209914885ull);
    vlSelf->agnus__DOT__bpd1__DOT__dmaena_delayed = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 5763715320330869061ull);
    vlSelf->agnus__DOT__bpd1__DOT__vdiwstrt = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 9096488342025458964ull);
    vlSelf->agnus__DOT__bpd1__DOT__vdiwstop = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 7790785746889137300ull);
    vlSelf->agnus__DOT__bpd1__DOT__vdiwena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14206822834415175863ull);
    vlSelf->agnus__DOT__bpd1__DOT__hdiwstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 4764104220547292791ull);
    vlSelf->agnus__DOT__bpd1__DOT__hdiwstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 17337175721743561355ull);
    vlSelf->agnus__DOT__bpd1__DOT__bplptr_sel = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 11220032413456370710ull);
    vlSelf->agnus__DOT__bpd1__DOT__bp_fmode12 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3222715009035336368ull);
    vlSelf->agnus__DOT__bpd1__DOT__soft_start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1546508827801717366ull);
    vlSelf->agnus__DOT__bpd1__DOT__soft_stop = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4744118045789146173ull);
    vlSelf->agnus__DOT__bpd1__DOT__hard_start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16589075142024702686ull);
    vlSelf->agnus__DOT__bpd1__DOT__hard_stop = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5724534772304889582ull);
    vlSelf->agnus__DOT__bpd1__DOT__ddfseq_match = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8632579015089128617ull);
    vlSelf->agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 6242429162723775706ull);
    vlSelf->agnus__DOT__bpd1__DOT__unnamedblk1__DOT__cur_hdiwstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 11230194161269911268ull);
    vlSelf->agnus__DOT__bpd1__DOT__unnamedblk1__DOT__best_hdiwstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 13987383139494733468ull);
    vlSelf->agnus__DOT__bpd1__DOT__unnamedblk1__DOT__cur_hdiwstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 11675762972133698606ull);
    vlSelf->agnus__DOT__bpd1__DOT__unnamedblk1__DOT__d_hde = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 6915361231682204750ull);
    vlSelf->agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_9 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2259662547462329146ull);
    vlSelf->agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_14 = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 7017555858976139327ull);
    vlSelf->agnus__DOT__bpd1__DOT____VdfgRegularize_hd6dcd5c0_0_17 = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 6859987437340456243ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus__DOT__spr1__DOT__sprpth[__Vi0] = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12966953599151924904ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus__DOT__spr1__DOT__sprptl[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 3658686951247228326ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus__DOT__spr1__DOT__sprpos[__Vi0] = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 11088788462833655640ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus__DOT__spr1__DOT__sprposh[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8994938394828108344ull);
    }
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus__DOT__spr1__DOT__sprctl[__Vi0] = VL_SCOPED_RAND_RESET_I(12, __VscopeHash, 1625311198930242804ull);
    }
    vlSelf->agnus__DOT__spr1__DOT__vstop = VL_SCOPED_RAND_RESET_I(10, __VscopeHash, 477211125662738838ull);
    vlSelf->agnus__DOT__spr1__DOT__sprite = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 18117966402023524660ull);
    vlSelf->agnus__DOT__spr1__DOT__newptr = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 4772481977206090607ull);
    vlSelf->agnus__DOT__spr1__DOT__enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5011552136341471560ull);
    vlSelf->agnus__DOT__spr1__DOT__sprvstop = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10705153464702542655ull);
    vlSelf->agnus__DOT__spr1__DOT__sprdmastate = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4680497820313718559ull);
    for (int __Vi0 = 0; __Vi0 < 8; ++__Vi0) {
        vlSelf->agnus__DOT__spr1__DOT__dmastate_mem[__Vi0] = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3138205193745112456ull);
    }
    vlSelf->agnus__DOT__spr1__DOT__dmastate_in = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13415884731813919991ull);
    vlSelf->agnus__DOT__spr1__DOT__sprsel = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 6399855358238080078ull);
    vlSelf->agnus__DOT__spr1__DOT__fmode = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8658333363087326336ull);
    vlSelf->agnus__DOT__spr1__DOT__ptsel = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 8767989599607940316ull);
    vlSelf->agnus__DOT__spr1__DOT__pcsel = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 13866424442264814656ull);
    vlSelf->agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1694978553393547686ull);
    vlSelf->agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2430184622871574873ull);
    vlSelf->agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_3 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 7315341182060404609ull);
    vlSelf->agnus__DOT__spr1__DOT____VdfgRegularize_h936bceaf_0_4 = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 10348385507240525345ull);
    vlSelf->agnus__DOT__cp1__DOT__cop1lch = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 1677420815034504074ull);
    vlSelf->agnus__DOT__cp1__DOT__cop1lcl = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 5442556827868858885ull);
    vlSelf->agnus__DOT__cp1__DOT__cop2lch = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 12874104168649196356ull);
    vlSelf->agnus__DOT__cp1__DOT__cop2lcl = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 6443412817383132744ull);
    vlSelf->agnus__DOT__cp1__DOT__cdang = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17823193753619934642ull);
    vlSelf->agnus__DOT__cp1__DOT__ir1 = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 17808089061356006106ull);
    vlSelf->agnus__DOT__cp1__DOT__ir2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 942644815794413132ull);
    vlSelf->agnus__DOT__cp1__DOT__copper_state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 1512310358257892723ull);
    vlSelf->agnus__DOT__cp1__DOT__copper_next = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 921582852495510536ull);
    vlSelf->agnus__DOT__cp1__DOT__strobe1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5846182833296563437ull);
    vlSelf->agnus__DOT__cp1__DOT__strobe2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14480331896801490505ull);
    vlSelf->agnus__DOT__cp1__DOT__strobe = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11161968472080347321ull);
    vlSelf->agnus__DOT__cp1__DOT__skip_flag = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5507845269170764289ull);
    vlSelf->agnus__DOT__cp1__DOT__selins = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13749614455881082776ull);
    vlSelf->agnus__DOT__cp1__DOT__selreg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 700359737509815735ull);
    vlSelf->agnus__DOT__cp1__DOT__skip = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11015631517996647955ull);
    vlSelf->agnus__DOT__cp1__DOT__enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 344243745216257795ull);
    vlSelf->agnus__DOT__cp1__DOT__dma_ack = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11861550345972559957ull);
    vlSelf->agnus__DOT__cp1__DOT__beam_match = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13949372814278409860ull);
    vlSelf->agnus__DOT__cp1__DOT__beam_match_skip = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11535713486401708710ull);
    vlSelf->agnus__DOT__cp1__DOT__beam_match_wait = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14531294400504967877ull);
    vlSelf->agnus__DOT__cp1__DOT__bus_ena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8729998517524828676ull);
    vlSelf->agnus__DOT__cp1__DOT__bus_blk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10818282415742656067ull);
    vlSelf->agnus__DOT__cp1__DOT__copjmp1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5565927338494131076ull);
    vlSelf->agnus__DOT__cp1__DOT__copjmp2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14066987711019891832ull);
    vlSelf->agnus__DOT__bl1__DOT__bltcon0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5345946903330534915ull);
    vlSelf->agnus__DOT__bl1__DOT__used = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5783811259545075525ull);
    vlSelf->agnus__DOT__bl1__DOT__enad = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1079704651964505634ull);
    vlSelf->agnus__DOT__bl1__DOT__bltcon1 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6889898280844912813ull);
    vlSelf->agnus__DOT__bl1__DOT__desc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 6364639984958737720ull);
    vlSelf->agnus__DOT__bl1__DOT__ife = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14081117070761472488ull);
    vlSelf->agnus__DOT__bl1__DOT__efe = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2046296706248358984ull);
    vlSelf->agnus__DOT__bl1__DOT__bltafwm = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11692936795073910695ull);
    vlSelf->agnus__DOT__bl1__DOT__bltalwm = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9137295083688869560ull);
    vlSelf->agnus__DOT__bl1__DOT__bltadat = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13314603494972800102ull);
    vlSelf->agnus__DOT__bl1__DOT__bltbdat = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9518854173672316658ull);
    vlSelf->agnus__DOT__bl1__DOT__bltcdat = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6777757467433046500ull);
    vlSelf->agnus__DOT__bl1__DOT__bltaold = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9952395777751794448ull);
    vlSelf->agnus__DOT__bl1__DOT__bltbold = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 252437303958011864ull);
    vlSelf->agnus__DOT__bl1__DOT__bltahold = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 726230085408017806ull);
    vlSelf->agnus__DOT__bl1__DOT__bltbhold = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8728339548465150464ull);
    vlSelf->agnus__DOT__bl1__DOT__bltdhold = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11293480231569474480ull);
    vlSelf->agnus__DOT__bl1__DOT__width = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 14709088447335956145ull);
    vlSelf->agnus__DOT__bl1__DOT__height = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 15689101222648225176ull);
    vlSelf->agnus__DOT__bl1__DOT__blt_state = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 13838495778070905310ull);
    vlSelf->agnus__DOT__bl1__DOT__blt_next = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 3214503890342455329ull);
    vlSelf->agnus__DOT__bl1__DOT__enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2212066187973375299ull);
    vlSelf->agnus__DOT__bl1__DOT__chsel = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 15142588891401656751ull);
    vlSelf->agnus__DOT__bl1__DOT__ptrsel = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 17291388412077287213ull);
    vlSelf->agnus__DOT__bl1__DOT__modsel = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 4856101856277931316ull);
    vlSelf->agnus__DOT__bl1__DOT__enaptr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1304951216755143876ull);
    vlSelf->agnus__DOT__bl1__DOT__incptr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17396340093234879218ull);
    vlSelf->agnus__DOT__bl1__DOT__decptr = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11025611922026714956ull);
    vlSelf->agnus__DOT__bl1__DOT__addmod = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15282923329336466071ull);
    vlSelf->agnus__DOT__bl1__DOT__submod = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17675783852249077491ull);
    vlSelf->agnus__DOT__bl1__DOT__decbsh = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 18053407637224035850ull);
    vlSelf->agnus__DOT__bl1__DOT__sign = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7293414616869759568ull);
    vlSelf->agnus__DOT__bl1__DOT__sign_del = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14069911433774178424ull);
    vlSelf->agnus__DOT__bl1__DOT__first_pixel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 2360585829636783482ull);
    vlSelf->agnus__DOT__bl1__DOT__first_line_pixel = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13896810860152171945ull);
    vlSelf->agnus__DOT__bl1__DOT__start = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9503566179701393348ull);
    vlSelf->agnus__DOT__bl1__DOT__next_word = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1469743247965564481ull);
    vlSelf->agnus__DOT__bl1__DOT__store_result = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14781324132086013651ull);
    vlSelf->agnus__DOT__bl1__DOT__pipeline_full = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16125860878514937625ull);
    vlSelf->agnus__DOT__bl1__DOT__first_word = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12747355099635612195ull);
    vlSelf->agnus__DOT__bl1__DOT__first_word_del = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16459255724610657097ull);
    vlSelf->agnus__DOT__bl1__DOT__last_word_del = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10925075453409134780ull);
    vlSelf->agnus__DOT__bl1__DOT__minterm_out = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 6593386879471548626ull);
    vlSelf->agnus__DOT__bl1__DOT__fill_out = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13726499691097962883ull);
    vlSelf->agnus__DOT__bl1__DOT__fcy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 8040330682664498667ull);
    vlSelf->agnus__DOT__bl1__DOT__width_cnt = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 9585039514870691801ull);
    vlSelf->agnus__DOT__bl1__DOT__width_cnt_dec = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1386739042804864834ull);
    vlSelf->agnus__DOT__bl1__DOT__width_cnt_rld = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4634351726000355465ull);
    vlSelf->agnus__DOT__bl1__DOT__height_cnt = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 5290444481204837299ull);
    vlSelf->agnus__DOT__bl1__DOT__bltamask = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 17622159604269663956ull);
    vlSelf->agnus__DOT__bl1__DOT__shiftbout = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 16470701701282266433ull);
    vlSelf->agnus__DOT__bl1__DOT__bltbold_init = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15035021126257963398ull);
    vlSelf->agnus__DOT__bl1__DOT__bltbdat_wrtn = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11799897838392303954ull);
    vlSelf->agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_7 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3300836691454960282ull);
    vlSelf->agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_8 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10879414400594862433ull);
    vlSelf->agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_9 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 599797458372950173ull);
    vlSelf->agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_11 = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 2251897511429011231ull);
    vlSelf->agnus__DOT__bl1__DOT____VdfgRegularize_h49fba040_0_12 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15124754900292554763ull);
    vlSelf->agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_new = VL_SCOPED_RAND_RESET_Q(36, __VscopeHash, 16508219154691131319ull);
    vlSelf->agnus__DOT__bl1__DOT__barrel_shifter_A__DOT__shifted_old = VL_SCOPED_RAND_RESET_Q(36, __VscopeHash, 7435440565018547099ull);
    vlSelf->agnus__DOT__bl1__DOT__bltmt1__DOT__mt0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11451775147172772197ull);
    vlSelf->agnus__DOT__bl1__DOT__bltmt1__DOT__mt1 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 8947068214283676231ull);
    vlSelf->agnus__DOT__bl1__DOT__bltmt1__DOT__mt2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11777182587303454154ull);
    vlSelf->agnus__DOT__bl1__DOT__bltmt1__DOT__mt3 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 12910999001487735449ull);
    vlSelf->agnus__DOT__bl1__DOT__bltmt1__DOT__mt4 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 5973049573893695032ull);
    vlSelf->agnus__DOT__bl1__DOT__bltmt1__DOT__mt5 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 271563364546694912ull);
    vlSelf->agnus__DOT__bl1__DOT__bltmt1__DOT__mt6 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 13420151242920816887ull);
    vlSelf->agnus__DOT__bl1__DOT__bltmt1__DOT__mt7 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10317958732988486981ull);
    vlSelf->agnus__DOT__bl1__DOT__bltfl1__DOT__carry = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 9382428972324750539ull);
    vlSelf->agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_sel = VL_SCOPED_RAND_RESET_I(2, __VscopeHash, 13806700362296029447ull);
    vlSelf->agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptr_in = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 10859315427295818368ull);
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->agnus__DOT__bl1__DOT__address_generator_1__DOT__bltpth[__Vi0] = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 10264379493842170263ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->agnus__DOT__bl1__DOT__address_generator_1__DOT__bltptl[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 6183014570287964617ull);
    }
    for (int __Vi0 = 0; __Vi0 < 4; ++__Vi0) {
        vlSelf->agnus__DOT__bl1__DOT__address_generator_1__DOT__bltmod[__Vi0] = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 6447130108696542418ull);
    }
    vlSelf->agnus__DOT__bl1__DOT__address_generator_1__DOT__newptr = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 12295347185269059818ull);
    vlSelf->agnus__DOT__bl1__DOT__address_generator_1__DOT____VdfgRegularize_h62043dcc_0_0 = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 6676934902185484143ull);
    vlSelf->agnus__DOT__bc1__DOT__long_frame = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15869183947469134859ull);
    vlSelf->agnus__DOT__bc1__DOT__long_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4708759725791338724ull);
    vlSelf->agnus__DOT__bc1__DOT__beamcon0_reg = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 7709693033183791690ull);
    vlSelf->agnus__DOT__bc1__DOT__ersy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9592967055789398553ull);
    vlSelf->agnus__DOT__bc1__DOT__pal = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16317861068574925758ull);
    vlSelf->agnus__DOT__bc1__DOT__htotal_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3405173986979020959ull);
    vlSelf->agnus__DOT__bc1__DOT__hsstrt_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 8296427672450592294ull);
    vlSelf->agnus__DOT__bc1__DOT__hsstop_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 5515166323486767867ull);
    vlSelf->agnus__DOT__bc1__DOT__hcenter_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 5583714993002971873ull);
    vlSelf->agnus__DOT__bc1__DOT__hbstrt_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 10217528143644429901ull);
    vlSelf->agnus__DOT__bc1__DOT__hbstop_reg = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 1791277140254638521ull);
    vlSelf->agnus__DOT__bc1__DOT__vtotal_reg = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 10246933229264339032ull);
    vlSelf->agnus__DOT__bc1__DOT__vsstrt_reg = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 5210119843421710251ull);
    vlSelf->agnus__DOT__bc1__DOT__vsstop_reg = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 3075642343684757372ull);
    vlSelf->agnus__DOT__bc1__DOT__vbstop_reg = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 1444694718230587838ull);
    vlSelf->agnus__DOT__bc1__DOT__hsstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 4419455903791275168ull);
    vlSelf->agnus__DOT__bc1__DOT__hsstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 3412310109671746886ull);
    vlSelf->agnus__DOT__bc1__DOT__hcenter = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 16777473033398125626ull);
    vlSelf->agnus__DOT__bc1__DOT__hbstrt = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 16389143443402318150ull);
    vlSelf->agnus__DOT__bc1__DOT__hbstop = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 11887585022896641435ull);
    vlSelf->agnus__DOT__bc1__DOT__vsstrt = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 17589460525277355727ull);
    vlSelf->agnus__DOT__bc1__DOT__vsstop = VL_SCOPED_RAND_RESET_I(11, __VscopeHash, 17893543805783298644ull);
    vlSelf->agnus__DOT__bc1__DOT__end_of_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14413040408833212480ull);
    vlSelf->agnus__DOT__bc1__DOT__hpos_r = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 5419064103824033832ull);
    vlSelf->agnus__DOT__bc1__DOT__vpos_inc = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16930063182413635963ull);
    vlSelf->agnus__DOT__bc1__DOT__last_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10034723299864537790ull);
    vlSelf->agnus__DOT__bc1__DOT__vpos_equ_vtotal = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 12797999081131817790ull);
    vlSelf->agnus__DOT__bc1__DOT__extra_line = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 17668367499316281854ull);
    vlSelf->agnus__DOT__bc1__DOT__vser = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 344468795136251196ull);
    vlSelf->__VdfgRegularize_h3c5bdd75_0_2 = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 10368470549122950936ull);
    vlSelf->__Vtrigprevexpr___TOP__agnus__DOT__bl1__DOT__bltfl1__DOT__carry__0 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10493999852400699985ull);
    vlSelf->__VstlDidInit = 0;
    vlSelf->__Vtrigprevexpr___TOP__agnus__DOT__bl1__DOT__bltfl1__DOT__carry__1 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 11129884286440650772ull);
    vlSelf->__Vtrigprevexpr___TOP__clk__0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9526919608049418986ull);
    vlSelf->__VactDidInit = 0;
}
