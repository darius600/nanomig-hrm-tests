// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_copper.h for the primary calling header

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper_agnus_copper.h"

VL_ATTR_COLD void Vagnus_copper_agnus_copper___ctor_var_reset(Vagnus_copper_agnus_copper* vlSelf) {
    VL_DEBUG_IF(VL_DBG_MSGF("+      Vagnus_copper_agnus_copper___ctor_var_reset\n"); );
    Vagnus_copper__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    auto& vlSelfRef = std::ref(*vlSelf).get();
    // Body
    const uint64_t __VscopeHash = VL_MURMUR64_HASH(vlSelf->name());
    vlSelf->clk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16707436170211756652ull);
    vlSelf->clk7_en = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1674876599471182170ull);
    vlSelf->reset = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9928399931838511862ull);
    vlSelf->ecs = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15277934814765900103ull);
    vlSelf->reqdma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3715291408256094165ull);
    vlSelf->ackdma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11778218106920099093ull);
    vlSelf->enadma = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 254979257843725267ull);
    vlSelf->sof = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7496247244052230992ull);
    vlSelf->blit_busy = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5165799694477622898ull);
    vlSelf->vpos = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 12586444320487401337ull);
    vlSelf->hpos = VL_SCOPED_RAND_RESET_I(9, __VscopeHash, 2050750607729723554ull);
    vlSelf->data_in = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 10574596302020702150ull);
    vlSelf->reg_address_in = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 13159730603054898824ull);
    vlSelf->reg_address_out = VL_SCOPED_RAND_RESET_I(8, __VscopeHash, 9386968039567798889ull);
    vlSelf->address_out = VL_SCOPED_RAND_RESET_I(20, __VscopeHash, 16520239430411259709ull);
    vlSelf->copper_state_dbg = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 2434778153253145750ull);
    vlSelf->dma_req_dbg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 4956901940480628737ull);
    vlSelf->bus_ena_dbg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 612941262805523989ull);
    vlSelf->clk_ena_dbg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16018568434773438160ull);
    vlSelf->__PVT__cop1lch = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 14133759670544955246ull);
    vlSelf->__PVT__cop1lcl = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 2475546770106199193ull);
    vlSelf->__PVT__cop2lch = VL_SCOPED_RAND_RESET_I(5, __VscopeHash, 17717911960921846036ull);
    vlSelf->__PVT__cop2lcl = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 6169534362413660676ull);
    vlSelf->__PVT__cdang = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5196165771503856182ull);
    vlSelf->__PVT__ir1 = VL_SCOPED_RAND_RESET_I(15, __VscopeHash, 18150930691869833703ull);
    vlSelf->__PVT__ir2 = VL_SCOPED_RAND_RESET_I(16, __VscopeHash, 3648361985656650314ull);
    vlSelf->__PVT__copper_state = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 1458403673385409063ull);
    vlSelf->__PVT__copper_next = VL_SCOPED_RAND_RESET_I(3, __VscopeHash, 8784654198097188918ull);
    vlSelf->__PVT__strobe1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14498377492294279922ull);
    vlSelf->__PVT__strobe2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14605266394105735597ull);
    vlSelf->__PVT__strobe = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11782777908846002911ull);
    vlSelf->__PVT__illegalreg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15454299167682883616ull);
    vlSelf->__PVT__skip_flag = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 14497226754407112963ull);
    vlSelf->selins = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10200500148160802466ull);
    vlSelf->selreg = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 16018844747452512572ull);
    vlSelf->__PVT__skip = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 15809514492369477403ull);
    vlSelf->__PVT__enable = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11030669854614834172ull);
    vlSelf->dma_req = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 1890691738561409054ull);
    vlSelf->__PVT__dma_ack = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 11884925740518628956ull);
    vlSelf->__PVT__beam_match = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 178508331554947912ull);
    vlSelf->__PVT__beam_match_skip = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 10410953399455901843ull);
    vlSelf->__PVT__beam_match_wait = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 3529121810421561926ull);
    vlSelf->__PVT__bus_ena = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7052229067147476328ull);
    vlSelf->__PVT__bus_blk = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 5902191778748528409ull);
    vlSelf->__PVT__copjmp1 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 13998325744772541813ull);
    vlSelf->__PVT__copjmp2 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 9079013559656754673ull);
    vlSelf->__VdfgRegularize_he23179ad_0_0 = VL_SCOPED_RAND_RESET_I(1, __VscopeHash, 7883692881766799427ull);
}
