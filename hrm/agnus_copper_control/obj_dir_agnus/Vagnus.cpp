// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus__pch.h"

//============================================================
// Constructors

Vagnus::Vagnus(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , cck{vlSymsp->TOP.cck}
    , clk7_en{vlSymsp->TOP.clk7_en}
    , reset{vlSymsp->TOP.reset}
    , aen{vlSymsp->TOP.aen}
    , rd{vlSymsp->TOP.rd}
    , hwr{vlSymsp->TOP.hwr}
    , lwr{vlSymsp->TOP.lwr}
    , address_in{vlSymsp->TOP.address_in}
    , reg_address_out{vlSymsp->TOP.reg_address_out}
    , cpu_custom{vlSymsp->TOP.cpu_custom}
    , dbr{vlSymsp->TOP.dbr}
    , dbwe{vlSymsp->TOP.dbwe}
    , refresh{vlSymsp->TOP.refresh}
    , _hsync{vlSymsp->TOP._hsync}
    , _vsync{vlSymsp->TOP._vsync}
    , _csync{vlSymsp->TOP._csync}
    , field1{vlSymsp->TOP.field1}
    , lace{vlSymsp->TOP.lace}
    , hblank{vlSymsp->TOP.hblank}
    , vblank{vlSymsp->TOP.vblank}
    , hde{vlSymsp->TOP.hde}
    , sol{vlSymsp->TOP.sol}
    , sof{vlSymsp->TOP.sof}
    , vbl_int{vlSymsp->TOP.vbl_int}
    , strhor_denise{vlSymsp->TOP.strhor_denise}
    , strhor_paula{vlSymsp->TOP.strhor_paula}
    , harddis{vlSymsp->TOP.harddis}
    , varbeamen{vlSymsp->TOP.varbeamen}
    , int3{vlSymsp->TOP.int3}
    , audio_dmal{vlSymsp->TOP.audio_dmal}
    , audio_dmas{vlSymsp->TOP.audio_dmas}
    , disk_dmal{vlSymsp->TOP.disk_dmal}
    , disk_dmas{vlSymsp->TOP.disk_dmas}
    , bls{vlSymsp->TOP.bls}
    , ntsc{vlSymsp->TOP.ntsc}
    , a1k{vlSymsp->TOP.a1k}
    , ecs{vlSymsp->TOP.ecs}
    , aga{vlSymsp->TOP.aga}
    , floppy_speed{vlSymsp->TOP.floppy_speed}
    , data_in{vlSymsp->TOP.data_in}
    , data_out{vlSymsp->TOP.data_out}
    , htotal{vlSymsp->TOP.htotal}
    , address_out{vlSymsp->TOP.address_out}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vagnus::Vagnus(const char* _vcname__)
    : Vagnus(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus::~Vagnus() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus___024root___eval_debug_assertions(Vagnus___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus___024root___eval_static(Vagnus___024root* vlSelf);
void Vagnus___024root___eval_initial(Vagnus___024root* vlSelf);
void Vagnus___024root___eval_settle(Vagnus___024root* vlSelf);
void Vagnus___024root___eval(Vagnus___024root* vlSelf);

void Vagnus::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus___024root___eval_static(&(vlSymsp->TOP));
        Vagnus___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus::eventsPending() { return false; }

uint64_t Vagnus::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus___024root___eval_final(Vagnus___024root* vlSelf);

VL_ATTR_COLD void Vagnus::final() {
    Vagnus___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus::hierName() const { return vlSymsp->name(); }
const char* Vagnus::modelName() const { return "Vagnus"; }
unsigned Vagnus::threads() const { return 1; }
void Vagnus::prepareClone() const { contextp()->prepareClone(); }
void Vagnus::atClone() const {
    contextp()->threadPoolpOnClone();
}
