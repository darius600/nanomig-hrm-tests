// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vagnus_copper__Syms.h"


void Vagnus_copper___024root__trace_chg_0_sub_0(Vagnus_copper___024root* vlSelf, VerilatedVcd::Buffer* bufp);

void Vagnus_copper___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_chg_0\n"); );
    // Init
    Vagnus_copper___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vagnus_copper___024root*>(voidSelf);
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (VL_UNLIKELY(!vlSymsp->__Vm_activity)) return;
    // Body
    Vagnus_copper___024root__trace_chg_0_sub_0((&vlSymsp->TOP), bufp);
}

void Vagnus_copper___024root__trace_chg_0_sub_0(Vagnus_copper___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_chg_0_sub_0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode + 1);
    // Body
    if (VL_UNLIKELY(((vlSelfRef.__Vm_traceActivity[1U] 
                      | vlSelfRef.__Vm_traceActivity
                      [2U])))) {
        bufp->chgBit(oldp+0,(vlSymsp->TOP__agnus_copper.__PVT__illegalreg));
        bufp->chgBit(oldp+1,(vlSymsp->TOP__agnus_copper.__PVT__skip));
        bufp->chgBit(oldp+2,(vlSymsp->TOP__agnus_copper.__PVT__enable));
        bufp->chgBit(oldp+3,(vlSymsp->TOP__agnus_copper.__PVT__dma_ack));
        bufp->chgBit(oldp+4,(vlSymsp->TOP__agnus_copper.__PVT__beam_match_skip));
    }
    if (VL_UNLIKELY((vlSelfRef.__Vm_traceActivity[2U]))) {
        bufp->chgIData(oldp+5,(vlSymsp->TOP__agnus_copper.address_out),20);
        bufp->chgCData(oldp+6,(vlSymsp->TOP__agnus_copper.__PVT__copper_state),3);
        bufp->chgBit(oldp+7,(vlSymsp->TOP__agnus_copper.__PVT__bus_ena));
        bufp->chgCData(oldp+8,(vlSymsp->TOP__agnus_copper.__PVT__cop1lch),5);
        bufp->chgSData(oldp+9,(vlSymsp->TOP__agnus_copper.__PVT__cop1lcl),15);
        bufp->chgCData(oldp+10,(vlSymsp->TOP__agnus_copper.__PVT__cop2lch),5);
        bufp->chgSData(oldp+11,(vlSymsp->TOP__agnus_copper.__PVT__cop2lcl),15);
        bufp->chgBit(oldp+12,(vlSymsp->TOP__agnus_copper.__PVT__cdang));
        bufp->chgSData(oldp+13,(vlSymsp->TOP__agnus_copper.__PVT__ir1),15);
        bufp->chgSData(oldp+14,(vlSymsp->TOP__agnus_copper.__PVT__ir2),16);
        bufp->chgBit(oldp+15,(vlSymsp->TOP__agnus_copper.__PVT__strobe1));
        bufp->chgBit(oldp+16,(vlSymsp->TOP__agnus_copper.__PVT__strobe2));
        bufp->chgBit(oldp+17,(vlSymsp->TOP__agnus_copper.__PVT__strobe));
        bufp->chgBit(oldp+18,(vlSymsp->TOP__agnus_copper.__PVT__skip_flag));
        bufp->chgBit(oldp+19,(vlSymsp->TOP__agnus_copper.__PVT__beam_match));
        bufp->chgBit(oldp+20,(vlSymsp->TOP__agnus_copper.__PVT__beam_match_wait));
        bufp->chgBit(oldp+21,(vlSymsp->TOP__agnus_copper.__PVT__bus_blk));
        bufp->chgBit(oldp+22,(vlSymsp->TOP__agnus_copper.__PVT__copjmp1));
        bufp->chgBit(oldp+23,(vlSymsp->TOP__agnus_copper.__PVT__copjmp2));
    }
    bufp->chgBit(oldp+24,(vlSelfRef.clk));
    bufp->chgBit(oldp+25,(vlSelfRef.clk7_en));
    bufp->chgBit(oldp+26,(vlSelfRef.reset));
    bufp->chgBit(oldp+27,(vlSelfRef.ecs));
    bufp->chgBit(oldp+28,(vlSelfRef.reqdma));
    bufp->chgBit(oldp+29,(vlSelfRef.ackdma));
    bufp->chgBit(oldp+30,(vlSelfRef.enadma));
    bufp->chgBit(oldp+31,(vlSelfRef.sof));
    bufp->chgBit(oldp+32,(vlSelfRef.blit_busy));
    bufp->chgCData(oldp+33,(vlSelfRef.vpos),8);
    bufp->chgSData(oldp+34,(vlSelfRef.hpos),9);
    bufp->chgSData(oldp+35,(vlSelfRef.data_in),16);
    bufp->chgCData(oldp+36,(vlSelfRef.reg_address_in),8);
    bufp->chgCData(oldp+37,(vlSelfRef.reg_address_out),8);
    bufp->chgIData(oldp+38,(vlSelfRef.address_out),20);
    bufp->chgCData(oldp+39,(vlSelfRef.copper_state_dbg),3);
    bufp->chgBit(oldp+40,(vlSelfRef.dma_req_dbg));
    bufp->chgBit(oldp+41,(vlSelfRef.bus_ena_dbg));
    bufp->chgBit(oldp+42,(vlSelfRef.clk_ena_dbg));
    bufp->chgBit(oldp+43,(vlSymsp->TOP__agnus_copper.dma_req));
    bufp->chgCData(oldp+44,(((4U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                              ? ((2U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                                  ? ((1U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                                      ? 4U : (((~ (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2)) 
                                               & (IData)(vlSymsp->TOP__agnus_copper.__PVT__illegalreg))
                                               ? 6U
                                               : ((
                                                   (~ (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2)) 
                                                   & (IData)(vlSymsp->TOP__agnus_copper.__PVT__skip_flag))
                                                   ? 
                                                  ((IData)(vlSymsp->TOP__agnus_copper.__PVT__dma_ack)
                                                    ? 4U
                                                    : 6U)
                                                   : 
                                                  ((1U 
                                                    & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                                    ? 
                                                   ((IData)(vlSymsp->TOP__agnus_copper.__PVT__dma_ack)
                                                     ? 2U
                                                     : 6U)
                                                    : 
                                                   ((IData)(vlSymsp->TOP__agnus_copper.__PVT__dma_ack)
                                                     ? 4U
                                                     : 6U)))))
                                  : ((1U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                                      ? 4U : ((IData)(vlSymsp->TOP__agnus_copper.__PVT__dma_ack)
                                               ? 6U
                                               : 4U)))
                              : ((2U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                                  ? ((1U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                                      ? ((1U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                          ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__beam_match_skip)
                                              ? ((IData)(vlSelfRef.enadma)
                                                  ? 4U
                                                  : 3U)
                                              : ((IData)(vlSelfRef.enadma)
                                                  ? 4U
                                                  : 3U))
                                          : ((IData)(vlSymsp->TOP__agnus_copper.__PVT__beam_match_wait)
                                              ? ((IData)(vlSelfRef.enadma)
                                                  ? 4U
                                                  : 3U)
                                              : 3U))
                                      : ((IData)(vlSelfRef.enadma)
                                          ? 3U : 2U))
                                  : ((1U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                                      ? 4U : ((IData)(vlSymsp->TOP__agnus_copper.__PVT__dma_ack)
                                               ? 4U
                                               : 0U))))),3);
    bufp->chgBit(oldp+45,(vlSymsp->TOP__agnus_copper.selins));
    bufp->chgBit(oldp+46,(vlSymsp->TOP__agnus_copper.selreg));
    bufp->chgCData(oldp+47,(((((0x40U & (((0x80U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                           ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                              >> 6U)
                                           : ((IData)(vlSelfRef.hpos) 
                                              >> 8U)) 
                                         << 6U)) | 
                               (0x20U & (((0x40U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                           ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                              >> 5U)
                                           : ((IData)(vlSelfRef.hpos) 
                                              >> 7U)) 
                                         << 5U))) | 
                              ((0x10U & (((0x20U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                           ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                              >> 4U)
                                           : ((IData)(vlSelfRef.hpos) 
                                              >> 6U)) 
                                         << 4U)) | 
                               (8U & (((0x10U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                        ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                           >> 3U) : 
                                       ((IData)(vlSelfRef.hpos) 
                                        >> 5U)) << 3U)))) 
                             | ((4U & (((8U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                         ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                            >> 2U) : 
                                        ((IData)(vlSelfRef.hpos) 
                                         >> 4U)) << 2U)) 
                                | ((2U & (((4U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                            ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                               >> 1U)
                                            : ((IData)(vlSelfRef.hpos) 
                                               >> 3U)) 
                                          << 1U)) | 
                                   (1U & ((2U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                           ? (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1)
                                           : ((IData)(vlSelfRef.hpos) 
                                              >> 2U))))))),7);
    bufp->chgCData(oldp+48,(((((0x80U & ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                         >> 7U)) | 
                               (0x40U & (((0x4000U 
                                           & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                           ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                              >> 0xdU)
                                           : ((IData)(vlSelfRef.vpos) 
                                              >> 6U)) 
                                         << 6U))) | 
                              ((0x20U & (((0x2000U 
                                           & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                           ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                              >> 0xcU)
                                           : ((IData)(vlSelfRef.vpos) 
                                              >> 5U)) 
                                         << 5U)) | 
                               (0x10U & (((0x1000U 
                                           & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                           ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                              >> 0xbU)
                                           : ((IData)(vlSelfRef.vpos) 
                                              >> 4U)) 
                                         << 4U)))) 
                             | (((8U & (((0x800U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                          ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                             >> 0xaU)
                                          : ((IData)(vlSelfRef.vpos) 
                                             >> 3U)) 
                                        << 3U)) | (4U 
                                                   & (((0x400U 
                                                        & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                                        ? 
                                                       ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                                        >> 9U)
                                                        : 
                                                       ((IData)(vlSelfRef.vpos) 
                                                        >> 2U)) 
                                                      << 2U))) 
                                | ((2U & (((0x200U 
                                            & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                            ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                               >> 8U)
                                            : ((IData)(vlSelfRef.vpos) 
                                               >> 1U)) 
                                          << 1U)) | 
                                   (1U & ((0x100U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                           ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                              >> 7U)
                                           : (IData)(vlSelfRef.vpos))))))),8);
}

void Vagnus_copper___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_cleanup\n"); );
    // Init
    Vagnus_copper___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vagnus_copper___024root*>(voidSelf);
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    vlSymsp->__Vm_activity = false;
    vlSymsp->TOP.__Vm_traceActivity[0U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[1U] = 0U;
    vlSymsp->TOP.__Vm_traceActivity[2U] = 0U;
}
