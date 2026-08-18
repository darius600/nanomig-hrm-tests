// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vagnus_copper__Syms.h"


VL_ATTR_COLD void Vagnus_copper___024root__trace_init_sub__TOP__agnus_copper__0(Vagnus_copper___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD void Vagnus_copper___024root__trace_init_sub__TOP__0(Vagnus_copper___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_init_sub__TOP__0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    const int c = vlSymsp->__Vm_baseCode;
    // Body
    tracep->pushPrefix("$rootio", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+25,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+26,0,"clk7_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+27,0,"reset",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+28,0,"ecs",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+29,0,"reqdma",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+30,0,"ackdma",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+31,0,"enadma",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+32,0,"sof",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+33,0,"blit_busy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+34,0,"vpos",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+35,0,"hpos",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+36,0,"data_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+37,0,"reg_address_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,1);
    tracep->declBus(c+38,0,"reg_address_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,1);
    tracep->declBus(c+39,0,"address_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 20,1);
    tracep->declBus(c+40,0,"copper_state_dbg",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+41,0,"dma_req_dbg",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+42,0,"bus_ena_dbg",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+43,0,"clk_ena_dbg",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->popPrefix();
    tracep->pushPrefix("agnus_copper", VerilatedTracePrefixType::SCOPE_MODULE);
    Vagnus_copper___024root__trace_init_sub__TOP__agnus_copper__0(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vagnus_copper___024root__trace_init_sub__TOP__agnus_copper__0(Vagnus_copper___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_init_sub__TOP__agnus_copper__0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    const int c = vlSymsp->__Vm_baseCode;
    // Body
    tracep->declBit(c+25,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+26,0,"clk7_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+27,0,"reset",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+28,0,"ecs",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+29,0,"reqdma",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+30,0,"ackdma",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+31,0,"enadma",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+32,0,"sof",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+33,0,"blit_busy",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+34,0,"vpos",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
    tracep->declBus(c+35,0,"hpos",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+36,0,"data_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+37,0,"reg_address_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,1);
    tracep->declBus(c+38,0,"reg_address_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,1);
    tracep->declBus(c+6,0,"address_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 20,1);
    tracep->declBus(c+7,0,"copper_state_dbg",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+44,0,"dma_req_dbg",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+8,0,"bus_ena_dbg",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+43,0,"clk_ena_dbg",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+50,0,"COP1LCH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+51,0,"COP1LCL",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+52,0,"COP2LCH",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+53,0,"COP2LCL",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+54,0,"COPCON",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+55,0,"COPINS",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+56,0,"COPJMP1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+57,0,"COPJMP2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+58,0,"RESET",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+59,0,"FETCH1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+60,0,"FETCH2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+61,0,"MOVE",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+62,0,"WAITSKIP1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+63,0,"WAITSKIP2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+9,0,"cop1lch",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 20,16);
    tracep->declBus(c+10,0,"cop1lcl",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,1);
    tracep->declBus(c+11,0,"cop2lch",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 20,16);
    tracep->declBus(c+12,0,"cop2lcl",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,1);
    tracep->declBit(c+13,0,"cdang",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+14,0,"ir1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,1);
    tracep->declBus(c+15,0,"ir2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+7,0,"copper_state",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+45,0,"copper_next",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBit(c+16,0,"strobe1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+17,0,"strobe2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+18,0,"strobe",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+1,0,"illegalreg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+19,0,"skip_flag",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+46,0,"selins",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+47,0,"selreg",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+2,0,"skip",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+3,0,"enable",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+44,0,"dma_req",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+4,0,"dma_ack",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+31,0,"dma_ena",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+20,0,"beam_match",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+5,0,"beam_match_skip",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+21,0,"beam_match_wait",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+43,0,"clk_ena",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+8,0,"bus_ena",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+22,0,"bus_blk",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+23,0,"copjmp1",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+24,0,"copjmp2",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+48,0,"horcmp",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,2);
    tracep->declBus(c+49,0,"vercmp",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 7,0);
}

VL_ATTR_COLD void Vagnus_copper___024root__trace_init_top(Vagnus_copper___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_init_top\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vagnus_copper___024root__trace_init_sub__TOP__0(vlSelf, tracep);
}

VL_ATTR_COLD void Vagnus_copper___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
VL_ATTR_COLD void Vagnus_copper___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vagnus_copper___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vagnus_copper___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/);

VL_ATTR_COLD void Vagnus_copper___024root__trace_register(Vagnus_copper___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_register\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    tracep->addConstCb(&Vagnus_copper___024root__trace_const_0, 0U, vlSelf);
    tracep->addFullCb(&Vagnus_copper___024root__trace_full_0, 0U, vlSelf);
    tracep->addChgCb(&Vagnus_copper___024root__trace_chg_0, 0U, vlSelf);
    tracep->addCleanupCb(&Vagnus_copper___024root__trace_cleanup, vlSelf);
}

VL_ATTR_COLD void Vagnus_copper___024root__trace_const_0_sub_0(Vagnus_copper___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vagnus_copper___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_const_0\n"); );
    // Init
    Vagnus_copper___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vagnus_copper___024root*>(voidSelf);
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vagnus_copper___024root__trace_const_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vagnus_copper___024root__trace_const_0_sub_0(Vagnus_copper___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_const_0_sub_0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullSData(oldp+50,(0x80U),9);
    bufp->fullSData(oldp+51,(0x82U),9);
    bufp->fullSData(oldp+52,(0x84U),9);
    bufp->fullSData(oldp+53,(0x86U),9);
    bufp->fullSData(oldp+54,(0x2eU),9);
    bufp->fullSData(oldp+55,(0x8cU),9);
    bufp->fullSData(oldp+56,(0x88U),9);
    bufp->fullSData(oldp+57,(0x8aU),9);
    bufp->fullCData(oldp+58,(0U),3);
    bufp->fullCData(oldp+59,(4U),3);
    bufp->fullCData(oldp+60,(6U),3);
    bufp->fullCData(oldp+61,(7U),3);
    bufp->fullCData(oldp+62,(2U),3);
    bufp->fullCData(oldp+63,(3U),3);
}

VL_ATTR_COLD void Vagnus_copper___024root__trace_full_0_sub_0(Vagnus_copper___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vagnus_copper___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_full_0\n"); );
    // Init
    Vagnus_copper___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vagnus_copper___024root*>(voidSelf);
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vagnus_copper___024root__trace_full_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vagnus_copper___024root__trace_full_0_sub_0(Vagnus_copper___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_copper___024root__trace_full_0_sub_0\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullBit(oldp+1,(vlSymsp->TOP__agnus_copper.__PVT__illegalreg));
    bufp->fullBit(oldp+2,(vlSymsp->TOP__agnus_copper.__PVT__skip));
    bufp->fullBit(oldp+3,(vlSymsp->TOP__agnus_copper.__PVT__enable));
    bufp->fullBit(oldp+4,(vlSymsp->TOP__agnus_copper.__PVT__dma_ack));
    bufp->fullBit(oldp+5,(vlSymsp->TOP__agnus_copper.__PVT__beam_match_skip));
    bufp->fullIData(oldp+6,(vlSymsp->TOP__agnus_copper.address_out),20);
    bufp->fullCData(oldp+7,(vlSymsp->TOP__agnus_copper.__PVT__copper_state),3);
    bufp->fullBit(oldp+8,(vlSymsp->TOP__agnus_copper.__PVT__bus_ena));
    bufp->fullCData(oldp+9,(vlSymsp->TOP__agnus_copper.__PVT__cop1lch),5);
    bufp->fullSData(oldp+10,(vlSymsp->TOP__agnus_copper.__PVT__cop1lcl),15);
    bufp->fullCData(oldp+11,(vlSymsp->TOP__agnus_copper.__PVT__cop2lch),5);
    bufp->fullSData(oldp+12,(vlSymsp->TOP__agnus_copper.__PVT__cop2lcl),15);
    bufp->fullBit(oldp+13,(vlSymsp->TOP__agnus_copper.__PVT__cdang));
    bufp->fullSData(oldp+14,(vlSymsp->TOP__agnus_copper.__PVT__ir1),15);
    bufp->fullSData(oldp+15,(vlSymsp->TOP__agnus_copper.__PVT__ir2),16);
    bufp->fullBit(oldp+16,(vlSymsp->TOP__agnus_copper.__PVT__strobe1));
    bufp->fullBit(oldp+17,(vlSymsp->TOP__agnus_copper.__PVT__strobe2));
    bufp->fullBit(oldp+18,(vlSymsp->TOP__agnus_copper.__PVT__strobe));
    bufp->fullBit(oldp+19,(vlSymsp->TOP__agnus_copper.__PVT__skip_flag));
    bufp->fullBit(oldp+20,(vlSymsp->TOP__agnus_copper.__PVT__beam_match));
    bufp->fullBit(oldp+21,(vlSymsp->TOP__agnus_copper.__PVT__beam_match_wait));
    bufp->fullBit(oldp+22,(vlSymsp->TOP__agnus_copper.__PVT__bus_blk));
    bufp->fullBit(oldp+23,(vlSymsp->TOP__agnus_copper.__PVT__copjmp1));
    bufp->fullBit(oldp+24,(vlSymsp->TOP__agnus_copper.__PVT__copjmp2));
    bufp->fullBit(oldp+25,(vlSelfRef.clk));
    bufp->fullBit(oldp+26,(vlSelfRef.clk7_en));
    bufp->fullBit(oldp+27,(vlSelfRef.reset));
    bufp->fullBit(oldp+28,(vlSelfRef.ecs));
    bufp->fullBit(oldp+29,(vlSelfRef.reqdma));
    bufp->fullBit(oldp+30,(vlSelfRef.ackdma));
    bufp->fullBit(oldp+31,(vlSelfRef.enadma));
    bufp->fullBit(oldp+32,(vlSelfRef.sof));
    bufp->fullBit(oldp+33,(vlSelfRef.blit_busy));
    bufp->fullCData(oldp+34,(vlSelfRef.vpos),8);
    bufp->fullSData(oldp+35,(vlSelfRef.hpos),9);
    bufp->fullSData(oldp+36,(vlSelfRef.data_in),16);
    bufp->fullCData(oldp+37,(vlSelfRef.reg_address_in),8);
    bufp->fullCData(oldp+38,(vlSelfRef.reg_address_out),8);
    bufp->fullIData(oldp+39,(vlSelfRef.address_out),20);
    bufp->fullCData(oldp+40,(vlSelfRef.copper_state_dbg),3);
    bufp->fullBit(oldp+41,(vlSelfRef.dma_req_dbg));
    bufp->fullBit(oldp+42,(vlSelfRef.bus_ena_dbg));
    bufp->fullBit(oldp+43,(vlSelfRef.clk_ena_dbg));
    bufp->fullBit(oldp+44,(vlSymsp->TOP__agnus_copper.dma_req));
    bufp->fullCData(oldp+45,(((4U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                               ? ((2U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                                   ? ((1U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__copper_state))
                                       ? 4U : (((~ (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2)) 
                                                & (IData)(vlSymsp->TOP__agnus_copper.__PVT__illegalreg))
                                                ? 6U
                                                : (
                                                   ((~ (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2)) 
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
    bufp->fullBit(oldp+46,(vlSymsp->TOP__agnus_copper.selins));
    bufp->fullBit(oldp+47,(vlSymsp->TOP__agnus_copper.selreg));
    bufp->fullCData(oldp+48,(((((0x40U & (((0x80U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
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
                                          << 5U))) 
                               | ((0x10U & (((0x20U 
                                              & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                              ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                                 >> 4U)
                                              : ((IData)(vlSelfRef.hpos) 
                                                 >> 6U)) 
                                            << 4U)) 
                                  | (8U & (((0x10U 
                                             & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                             ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                                >> 3U)
                                             : ((IData)(vlSelfRef.hpos) 
                                                >> 5U)) 
                                           << 3U)))) 
                              | ((4U & (((8U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                          ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                             >> 2U)
                                          : ((IData)(vlSelfRef.hpos) 
                                             >> 4U)) 
                                        << 2U)) | (
                                                   (2U 
                                                    & (((4U 
                                                         & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                                         ? 
                                                        ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                                         >> 1U)
                                                         : 
                                                        ((IData)(vlSelfRef.hpos) 
                                                         >> 3U)) 
                                                       << 1U)) 
                                                   | (1U 
                                                      & ((2U 
                                                          & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                                          ? (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1)
                                                          : 
                                                         ((IData)(vlSelfRef.hpos) 
                                                          >> 2U))))))),7);
    bufp->fullCData(oldp+49,(((((0x80U & ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                          >> 7U)) | 
                                (0x40U & (((0x4000U 
                                            & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                            ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                               >> 0xdU)
                                            : ((IData)(vlSelfRef.vpos) 
                                               >> 6U)) 
                                          << 6U))) 
                               | ((0x20U & (((0x2000U 
                                              & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                              ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                                 >> 0xcU)
                                              : ((IData)(vlSelfRef.vpos) 
                                                 >> 5U)) 
                                            << 5U)) 
                                  | (0x10U & (((0x1000U 
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
                                         << 3U)) | 
                                  (4U & (((0x400U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
                                           ? ((IData)(vlSymsp->TOP__agnus_copper.__PVT__ir1) 
                                              >> 9U)
                                           : ((IData)(vlSelfRef.vpos) 
                                              >> 2U)) 
                                         << 2U))) | 
                                 ((2U & (((0x200U & (IData)(vlSymsp->TOP__agnus_copper.__PVT__ir2))
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
