// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vagnus_bitplanedma__Syms.h"


void Vagnus_bitplanedma___024root__trace_chg_0_sub_0(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd::Buffer* bufp);

void Vagnus_bitplanedma___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_chg_0\n"); );
    // Init
    Vagnus_bitplanedma___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vagnus_bitplanedma___024root*>(voidSelf);
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    Vagnus_bitplanedma___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vagnus_bitplanedma___024root__trace_chg_0_sub_0(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_chg_0_sub_0\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    // Body
    if (VL_UNLIKELY(((vlSelfRef.__Vm_traceActivity[1U] 
                      | vlSelfRef.__Vm_traceActivity
                      [2U])))) {
        bufp->chgSData(oldp+0,(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan),15);
        bufp->chgSData(oldp+1,(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan),15);
        bufp->chgBit(oldp+2,(vlSelfRef.agnus_bitplanedma__DOT__shres));
        bufp->chgIData(oldp+3,(vlSelfRef.agnus_bitplanedma__DOT__newpt),20);
        bufp->chgCData(oldp+4,(vlSelfRef.agnus_bitplanedma__DOT__plane),5);
        bufp->chgBit(oldp+5,(((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                               ? ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9) 
                                  & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                     >> 1U)) : ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                                 ? (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9)
                                                 : (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfend)))));
        bufp->chgCData(oldp+6,(vlSelfRef.agnus_bitplanedma__DOT__bplptr_sel),3);
        bufp->chgBit(oldp+7,(vlSelfRef.agnus_bitplanedma__DOT__ddfseq_match));
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[2U]))) {
        bufp->chgCData(oldp+8,(vlSelfRef.agnus_bitplanedma__DOT__ddfstrt),7);
        bufp->chgCData(oldp+9,(vlSelfRef.agnus_bitplanedma__DOT__ddfstop),7);
        bufp->chgSData(oldp+10,(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod),15);
        bufp->chgSData(oldp+11,(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod),15);
        bufp->chgCData(oldp+12,(vlSelfRef.agnus_bitplanedma__DOT__bplcon0),6);
        bufp->chgCData(oldp+13,(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed),6);
        bufp->chgCData(oldp+14,(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delay[0]),6);
        bufp->chgCData(oldp+15,(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delay[1]),6);
        bufp->chgSData(oldp+16,(vlSelfRef.agnus_bitplanedma__DOT__fmode),16);
        bufp->chgBit(oldp+17,((1U & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                                     >> 4U))));
        bufp->chgCData(oldp+18,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[0]),5);
        bufp->chgCData(oldp+19,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[1]),5);
        bufp->chgCData(oldp+20,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[2]),5);
        bufp->chgCData(oldp+21,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[3]),5);
        bufp->chgCData(oldp+22,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[4]),5);
        bufp->chgCData(oldp+23,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[5]),5);
        bufp->chgCData(oldp+24,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[6]),5);
        bufp->chgCData(oldp+25,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[7]),5);
        bufp->chgSData(oldp+26,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[0]),15);
        bufp->chgSData(oldp+27,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[1]),15);
        bufp->chgSData(oldp+28,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[2]),15);
        bufp->chgSData(oldp+29,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[3]),15);
        bufp->chgSData(oldp+30,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[4]),15);
        bufp->chgSData(oldp+31,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[5]),15);
        bufp->chgSData(oldp+32,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[6]),15);
        bufp->chgSData(oldp+33,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[7]),15);
        bufp->chgBit(oldp+34,(vlSelfRef.agnus_bitplanedma__DOT__hardena));
        bufp->chgBit(oldp+35,(vlSelfRef.agnus_bitplanedma__DOT__softena));
        bufp->chgBit(oldp+36,(vlSelfRef.agnus_bitplanedma__DOT__ddfena));
        bufp->chgBit(oldp+37,(vlSelfRef.agnus_bitplanedma__DOT__ddfena_0));
        bufp->chgCData(oldp+38,(vlSelfRef.agnus_bitplanedma__DOT__ddfseq),5);
        bufp->chgBit(oldp+39,(vlSelfRef.agnus_bitplanedma__DOT__ddfrun));
        bufp->chgBit(oldp+40,(vlSelfRef.agnus_bitplanedma__DOT__ddfend));
        bufp->chgCData(oldp+41,(vlSelfRef.agnus_bitplanedma__DOT__dmaena_delayed),2);
        bufp->chgSData(oldp+42,(vlSelfRef.agnus_bitplanedma__DOT__vdiwstrt),11);
        bufp->chgSData(oldp+43,(vlSelfRef.agnus_bitplanedma__DOT__vdiwstop),11);
        bufp->chgBit(oldp+44,(vlSelfRef.agnus_bitplanedma__DOT__vdiwena));
        bufp->chgSData(oldp+45,(vlSelfRef.agnus_bitplanedma__DOT__hdiwstrt),9);
        bufp->chgSData(oldp+46,(vlSelfRef.agnus_bitplanedma__DOT__hdiwstop),9);
        bufp->chgBit(oldp+47,((0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
        bufp->chgBit(oldp+48,(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12));
        bufp->chgBit(oldp+49,((3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
        bufp->chgBit(oldp+50,(vlSelfRef.agnus_bitplanedma__DOT__soft_start));
        bufp->chgBit(oldp+51,(vlSelfRef.agnus_bitplanedma__DOT__soft_stop));
        bufp->chgBit(oldp+52,(vlSelfRef.agnus_bitplanedma__DOT__hard_start));
        bufp->chgBit(oldp+53,(vlSelfRef.agnus_bitplanedma__DOT__hard_stop));
        bufp->chgSData(oldp+54,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt),9);
        bufp->chgSData(oldp+55,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstrt),9);
        bufp->chgSData(oldp+56,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop),9);
        bufp->chgSData(oldp+57,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstop),9);
        bufp->chgSData(oldp+58,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde),11);
    }
    bufp->chgBit(oldp+59,(vlSelfRef.clk));
    bufp->chgBit(oldp+60,(vlSelfRef.clk7_en));
    bufp->chgBit(oldp+61,(vlSelfRef.reset));
    bufp->chgBit(oldp+62,(vlSelfRef.harddis));
    bufp->chgBit(oldp+63,(vlSelfRef.aga));
    bufp->chgBit(oldp+64,(vlSelfRef.ecs));
    bufp->chgBit(oldp+65,(vlSelfRef.a1k));
    bufp->chgBit(oldp+66,(vlSelfRef.sof));
    bufp->chgBit(oldp+67,(vlSelfRef.dmaena));
    bufp->chgSData(oldp+68,(vlSelfRef.vpos),11);
    bufp->chgSData(oldp+69,(vlSelfRef.hpos),9);
    bufp->chgBit(oldp+70,(vlSelfRef.hde));
    bufp->chgBit(oldp+71,(vlSelfRef.dma));
    bufp->chgCData(oldp+72,(vlSelfRef.reg_address_in),8);
    bufp->chgCData(oldp+73,(vlSelfRef.reg_address_out),8);
    bufp->chgSData(oldp+74,(vlSelfRef.data_in),16);
    bufp->chgIData(oldp+75,(vlSelfRef.address_out),20);
    bufp->chgCData(oldp+76,((0xfU & ((IData)(vlSelfRef.aga)
                                      ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)
                                      : ((7U == (7U 
                                                 & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))
                                          ? 4U : (7U 
                                                  & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))))),4);
    bufp->chgCData(oldp+77,((0x1fU & ((IData)(vlSelfRef.dma)
                                       ? (vlSelfRef.agnus_bitplanedma__DOT__newpt 
                                          >> 0xfU) : (IData)(vlSelfRef.data_in)))),5);
    bufp->chgSData(oldp+78,((0x7fffU & ((IData)(vlSelfRef.dma)
                                         ? vlSelfRef.agnus_bitplanedma__DOT__newpt
                                         : ((IData)(vlSelfRef.data_in) 
                                            >> 1U)))),15);
    bufp->chgBit(oldp+79,((0x49U == (IData)(vlSelfRef.reg_address_in))));
}

void Vagnus_bitplanedma___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_cleanup\n"); );
    // Init
    Vagnus_bitplanedma___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vagnus_bitplanedma___024root*>(voidSelf);
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[2U] = 0U;
}
