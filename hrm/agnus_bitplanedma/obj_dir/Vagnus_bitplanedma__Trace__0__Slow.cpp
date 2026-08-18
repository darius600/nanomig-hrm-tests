// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Tracing implementation internals
#include "verilated_vcd_c.h"
#include "Vagnus_bitplanedma__Syms.h"


VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_init_sub__TOP__0(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_init_sub__TOP__0\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    const int c = vlSymsp->__Vm_baseCode;
    // Body
    tracep->pushPrefix("$rootio", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+60,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+61,0,"clk7_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+62,0,"reset",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+63,0,"harddis",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+64,0,"aga",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+65,0,"ecs",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+66,0,"a1k",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+67,0,"sof",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+68,0,"dmaena",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+69,0,"vpos",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 10,0);
    tracep->declBus(c+70,0,"hpos",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBit(c+71,0,"hde",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+72,0,"dma",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+73,0,"reg_address_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,1);
    tracep->declBus(c+74,0,"reg_address_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,1);
    tracep->declBus(c+75,0,"data_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+76,0,"address_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 20,1);
    tracep->popPrefix();
    tracep->pushPrefix("agnus_bitplanedma", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBit(c+60,0,"clk",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+61,0,"clk7_en",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+62,0,"reset",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+63,0,"harddis",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+64,0,"aga",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+65,0,"ecs",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+66,0,"a1k",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+67,0,"sof",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+68,0,"dmaena",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+69,0,"vpos",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 10,0);
    tracep->declBus(c+70,0,"hpos",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBit(c+71,0,"hde",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+72,0,"dma",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+73,0,"reg_address_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,1);
    tracep->declBus(c+74,0,"reg_address_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,1);
    tracep->declBus(c+75,0,"data_in",-1, VerilatedTraceSigDirection::INPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBus(c+76,0,"address_out",-1, VerilatedTraceSigDirection::OUTPUT, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 20,1);
    tracep->declBus(c+81,0,"DIWSTRT_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+82,0,"DIWSTOP_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+83,0,"DIWHIGH_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+84,0,"BPLPTBASE_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+85,0,"DDFSTRT_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+86,0,"DDFSTOP_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+87,0,"BPL1MOD_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+88,0,"BPL2MOD_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+89,0,"BPLCON0_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+90,0,"FMODE_REG",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::PARAMETER, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+9,0,"ddfstrt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,2);
    tracep->declBus(c+10,0,"ddfstop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,2);
    tracep->declBus(c+91,0,"ddfdiff",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,2);
    tracep->declBus(c+92,0,"ddfdiff_masked",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 8,2);
    tracep->declBus(c+11,0,"bpl1mod",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,1);
    tracep->declBus(c+12,0,"bpl2mod",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,1);
    tracep->declBus(c+1,0,"bpl1mod_bscan",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,1);
    tracep->declBus(c+2,0,"bpl2mod_bscan",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,1);
    tracep->declBus(c+13,0,"bplcon0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->declBus(c+14,0,"bplcon0_delayed",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 5,0);
    tracep->pushPrefix("bplcon0_delay", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 2; ++i) {
        tracep->declBus(c+15+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 5,0);
    }
    tracep->popPrefix();
    tracep->declBus(c+17,0,"fmode",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 15,0);
    tracep->declBit(c+18,0,"hires",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+3,0,"shres",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+77,0,"bpu",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 3,0);
    tracep->declBus(c+4,0,"newpt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 20,1);
    tracep->pushPrefix("bplpth", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 8; ++i) {
        tracep->declBus(c+19+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 20,16);
    }
    tracep->popPrefix();
    tracep->pushPrefix("bplptl", VerilatedTracePrefixType::ARRAY_UNPACKED);
    for (int i = 0; i < 8; ++i) {
        tracep->declBus(c+27+i*1,0,"",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, true,(i+0), 15,1);
    }
    tracep->popPrefix();
    tracep->declBus(c+5,0,"plane",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+6,0,"mod",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+35,0,"hardena",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+36,0,"softena",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+37,0,"ddfena",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+38,0,"ddfena_0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+39,0,"ddfseq",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 4,0);
    tracep->declBit(c+40,0,"ddfrun",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+41,0,"ddfend",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+42,0,"dmaena_delayed",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 1,0);
    tracep->declBus(c+43,0,"vdiwstrt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 10,0);
    tracep->declBus(c+44,0,"vdiwstop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 10,0);
    tracep->declBit(c+45,0,"vdiwena",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBus(c+46,0,"hdiwstrt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+47,0,"hdiwstop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+7,0,"bplptr_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 2,0);
    tracep->declBus(c+78,0,"bplpth_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 20,16);
    tracep->declBus(c+79,0,"bplptl_in",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1, 15,1);
    tracep->declBit(c+80,0,"ddfstrt_sel",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+48,0,"bp_fmode0",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+49,0,"bp_fmode12",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+50,0,"bp_fmode3",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+51,0,"soft_start",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+52,0,"soft_stop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+53,0,"hard_start",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+54,0,"hard_stop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->declBit(c+8,0,"ddfseq_match",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::WIRE, VerilatedTraceSigType::LOGIC, false,-1);
    tracep->pushPrefix("unnamedblk1", VerilatedTracePrefixType::SCOPE_MODULE);
    tracep->declBus(c+55,0,"best_hdiwstrt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+56,0,"cur_hdiwstrt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+93,0,"prev_hdiwstrt",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+57,0,"best_hdiwstop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+58,0,"cur_hdiwstop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+94,0,"prev_hdiwstop",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 8,0);
    tracep->declBus(c+59,0,"d_hde",-1, VerilatedTraceSigDirection::NONE, VerilatedTraceSigKind::VAR, VerilatedTraceSigType::LOGIC, false,-1, 10,0);
    tracep->popPrefix();
    tracep->popPrefix();
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_init_top(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_init_top\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    Vagnus_bitplanedma___024root__trace_init_sub__TOP__0(vlSelf, tracep);
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vagnus_bitplanedma___024root__trace_chg_0(void* voidSelf, VerilatedVcd::Buffer* bufp);
void Vagnus_bitplanedma___024root__trace_cleanup(void* voidSelf, VerilatedVcd* /*unused*/);

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_register(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd* tracep) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_register\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    tracep->addConstCb(&Vagnus_bitplanedma___024root__trace_const_0, 0U, vlSelf);
    tracep->addFullCb(&Vagnus_bitplanedma___024root__trace_full_0, 0U, vlSelf);
    tracep->addChgCb(&Vagnus_bitplanedma___024root__trace_chg_0, 0U, vlSelf);
    tracep->addCleanupCb(&Vagnus_bitplanedma___024root__trace_cleanup, vlSelf);
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_const_0_sub_0(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_const_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_const_0\n"); );
    // Init
    Vagnus_bitplanedma___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vagnus_bitplanedma___024root*>(voidSelf);
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vagnus_bitplanedma___024root__trace_const_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_const_0_sub_0(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_const_0_sub_0\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullSData(oldp+81,(0x8eU),9);
    bufp->fullSData(oldp+82,(0x90U),9);
    bufp->fullSData(oldp+83,(0x1e4U),9);
    bufp->fullSData(oldp+84,(0xe0U),9);
    bufp->fullSData(oldp+85,(0x92U),9);
    bufp->fullSData(oldp+86,(0x94U),9);
    bufp->fullSData(oldp+87,(0x108U),9);
    bufp->fullSData(oldp+88,(0x10aU),9);
    bufp->fullSData(oldp+89,(0x100U),9);
    bufp->fullSData(oldp+90,(0x1fcU),9);
    bufp->fullCData(oldp+91,(vlSelfRef.agnus_bitplanedma__DOT__ddfdiff),7);
    bufp->fullCData(oldp+92,(vlSelfRef.agnus_bitplanedma__DOT__ddfdiff_masked),7);
    bufp->fullSData(oldp+93,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__prev_hdiwstrt),9);
    bufp->fullSData(oldp+94,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__prev_hdiwstop),9);
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_full_0_sub_0(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd::Buffer* bufp);

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_full_0(void* voidSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_full_0\n"); );
    // Init
    Vagnus_bitplanedma___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vagnus_bitplanedma___024root*>(voidSelf);
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    // Body
    Vagnus_bitplanedma___024root__trace_full_0_sub_0((&vlSymsp->TOP), bufp);
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_full_0_sub_0(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd::Buffer* bufp) {
    VL_DEBUG_IF(VL_DBG_MSGF("+    Vagnus_bitplanedma___024root__trace_full_0_sub_0\n"); );
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Init
    uint32_t* const oldp VL_ATTR_UNUSED = bufp->oldp(vlSymsp->__Vm_baseCode);
    // Body
    bufp->fullSData(oldp+1,(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod_bscan),15);
    bufp->fullSData(oldp+2,(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod_bscan),15);
    bufp->fullBit(oldp+3,(vlSelfRef.agnus_bitplanedma__DOT__shres));
    bufp->fullIData(oldp+4,(vlSelfRef.agnus_bitplanedma__DOT__newpt),20);
    bufp->fullCData(oldp+5,(vlSelfRef.agnus_bitplanedma__DOT__plane),5);
    bufp->fullBit(oldp+6,(((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_7)
                            ? ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9) 
                               & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfseq) 
                                  >> 1U)) : ((IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_11)
                                              ? (IData)(vlSelfRef.agnus_bitplanedma__DOT____VdfgRegularize_hd6dcd5c0_0_9)
                                              : (IData)(vlSelfRef.agnus_bitplanedma__DOT__ddfend)))));
    bufp->fullCData(oldp+7,(vlSelfRef.agnus_bitplanedma__DOT__bplptr_sel),3);
    bufp->fullBit(oldp+8,(vlSelfRef.agnus_bitplanedma__DOT__ddfseq_match));
    bufp->fullCData(oldp+9,(vlSelfRef.agnus_bitplanedma__DOT__ddfstrt),7);
    bufp->fullCData(oldp+10,(vlSelfRef.agnus_bitplanedma__DOT__ddfstop),7);
    bufp->fullSData(oldp+11,(vlSelfRef.agnus_bitplanedma__DOT__bpl1mod),15);
    bufp->fullSData(oldp+12,(vlSelfRef.agnus_bitplanedma__DOT__bpl2mod),15);
    bufp->fullCData(oldp+13,(vlSelfRef.agnus_bitplanedma__DOT__bplcon0),6);
    bufp->fullCData(oldp+14,(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed),6);
    bufp->fullCData(oldp+15,(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delay[0]),6);
    bufp->fullCData(oldp+16,(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delay[1]),6);
    bufp->fullSData(oldp+17,(vlSelfRef.agnus_bitplanedma__DOT__fmode),16);
    bufp->fullBit(oldp+18,((1U & ((IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed) 
                                  >> 4U))));
    bufp->fullCData(oldp+19,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[0]),5);
    bufp->fullCData(oldp+20,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[1]),5);
    bufp->fullCData(oldp+21,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[2]),5);
    bufp->fullCData(oldp+22,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[3]),5);
    bufp->fullCData(oldp+23,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[4]),5);
    bufp->fullCData(oldp+24,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[5]),5);
    bufp->fullCData(oldp+25,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[6]),5);
    bufp->fullCData(oldp+26,(vlSelfRef.agnus_bitplanedma__DOT__bplpth[7]),5);
    bufp->fullSData(oldp+27,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[0]),15);
    bufp->fullSData(oldp+28,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[1]),15);
    bufp->fullSData(oldp+29,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[2]),15);
    bufp->fullSData(oldp+30,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[3]),15);
    bufp->fullSData(oldp+31,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[4]),15);
    bufp->fullSData(oldp+32,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[5]),15);
    bufp->fullSData(oldp+33,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[6]),15);
    bufp->fullSData(oldp+34,(vlSelfRef.agnus_bitplanedma__DOT__bplptl[7]),15);
    bufp->fullBit(oldp+35,(vlSelfRef.agnus_bitplanedma__DOT__hardena));
    bufp->fullBit(oldp+36,(vlSelfRef.agnus_bitplanedma__DOT__softena));
    bufp->fullBit(oldp+37,(vlSelfRef.agnus_bitplanedma__DOT__ddfena));
    bufp->fullBit(oldp+38,(vlSelfRef.agnus_bitplanedma__DOT__ddfena_0));
    bufp->fullCData(oldp+39,(vlSelfRef.agnus_bitplanedma__DOT__ddfseq),5);
    bufp->fullBit(oldp+40,(vlSelfRef.agnus_bitplanedma__DOT__ddfrun));
    bufp->fullBit(oldp+41,(vlSelfRef.agnus_bitplanedma__DOT__ddfend));
    bufp->fullCData(oldp+42,(vlSelfRef.agnus_bitplanedma__DOT__dmaena_delayed),2);
    bufp->fullSData(oldp+43,(vlSelfRef.agnus_bitplanedma__DOT__vdiwstrt),11);
    bufp->fullSData(oldp+44,(vlSelfRef.agnus_bitplanedma__DOT__vdiwstop),11);
    bufp->fullBit(oldp+45,(vlSelfRef.agnus_bitplanedma__DOT__vdiwena));
    bufp->fullSData(oldp+46,(vlSelfRef.agnus_bitplanedma__DOT__hdiwstrt),9);
    bufp->fullSData(oldp+47,(vlSelfRef.agnus_bitplanedma__DOT__hdiwstop),9);
    bufp->fullBit(oldp+48,((0U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
    bufp->fullBit(oldp+49,(vlSelfRef.agnus_bitplanedma__DOT__bp_fmode12));
    bufp->fullBit(oldp+50,((3U == (3U & (IData)(vlSelfRef.agnus_bitplanedma__DOT__fmode)))));
    bufp->fullBit(oldp+51,(vlSelfRef.agnus_bitplanedma__DOT__soft_start));
    bufp->fullBit(oldp+52,(vlSelfRef.agnus_bitplanedma__DOT__soft_stop));
    bufp->fullBit(oldp+53,(vlSelfRef.agnus_bitplanedma__DOT__hard_start));
    bufp->fullBit(oldp+54,(vlSelfRef.agnus_bitplanedma__DOT__hard_stop));
    bufp->fullSData(oldp+55,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstrt),9);
    bufp->fullSData(oldp+56,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstrt),9);
    bufp->fullSData(oldp+57,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__best_hdiwstop),9);
    bufp->fullSData(oldp+58,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__cur_hdiwstop),9);
    bufp->fullSData(oldp+59,(vlSelfRef.agnus_bitplanedma__DOT__unnamedblk1__DOT__d_hde),11);
    bufp->fullBit(oldp+60,(vlSelfRef.clk));
    bufp->fullBit(oldp+61,(vlSelfRef.clk7_en));
    bufp->fullBit(oldp+62,(vlSelfRef.reset));
    bufp->fullBit(oldp+63,(vlSelfRef.harddis));
    bufp->fullBit(oldp+64,(vlSelfRef.aga));
    bufp->fullBit(oldp+65,(vlSelfRef.ecs));
    bufp->fullBit(oldp+66,(vlSelfRef.a1k));
    bufp->fullBit(oldp+67,(vlSelfRef.sof));
    bufp->fullBit(oldp+68,(vlSelfRef.dmaena));
    bufp->fullSData(oldp+69,(vlSelfRef.vpos),11);
    bufp->fullSData(oldp+70,(vlSelfRef.hpos),9);
    bufp->fullBit(oldp+71,(vlSelfRef.hde));
    bufp->fullBit(oldp+72,(vlSelfRef.dma));
    bufp->fullCData(oldp+73,(vlSelfRef.reg_address_in),8);
    bufp->fullCData(oldp+74,(vlSelfRef.reg_address_out),8);
    bufp->fullSData(oldp+75,(vlSelfRef.data_in),16);
    bufp->fullIData(oldp+76,(vlSelfRef.address_out),20);
    bufp->fullCData(oldp+77,((0xfU & ((IData)(vlSelfRef.aga)
                                       ? (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)
                                       : ((7U == (7U 
                                                  & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))
                                           ? 4U : (7U 
                                                   & (IData)(vlSelfRef.agnus_bitplanedma__DOT__bplcon0_delayed)))))),4);
    bufp->fullCData(oldp+78,((0x1fU & ((IData)(vlSelfRef.dma)
                                        ? (vlSelfRef.agnus_bitplanedma__DOT__newpt 
                                           >> 0xfU)
                                        : (IData)(vlSelfRef.data_in)))),5);
    bufp->fullSData(oldp+79,((0x7fffU & ((IData)(vlSelfRef.dma)
                                          ? vlSelfRef.agnus_bitplanedma__DOT__newpt
                                          : ((IData)(vlSelfRef.data_in) 
                                             >> 1U)))),15);
    bufp->fullBit(oldp+80,((0x49U == (IData)(vlSelfRef.reg_address_in))));
}
