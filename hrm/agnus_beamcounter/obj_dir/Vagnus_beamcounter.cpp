// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_beamcounter__pch.h"

//============================================================
// Constructors

Vagnus_beamcounter::Vagnus_beamcounter(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_beamcounter__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , cck{vlSymsp->TOP.cck}
    , clk7_en{vlSymsp->TOP.clk7_en}
    , reset{vlSymsp->TOP.reset}
    , ntsc{vlSymsp->TOP.ntsc}
    , aga{vlSymsp->TOP.aga}
    , ecs{vlSymsp->TOP.ecs}
    , a1k{vlSymsp->TOP.a1k}
    , reg_address_in{vlSymsp->TOP.reg_address_in}
    , _hsync{vlSymsp->TOP._hsync}
    , _vsync{vlSymsp->TOP._vsync}
    , field1{vlSymsp->TOP.field1}
    , lace{vlSymsp->TOP.lace}
    , _csync{vlSymsp->TOP._csync}
    , hblank{vlSymsp->TOP.hblank}
    , vblank{vlSymsp->TOP.vblank}
    , vbl{vlSymsp->TOP.vbl}
    , vblend{vlSymsp->TOP.vblend}
    , eol{vlSymsp->TOP.eol}
    , eof{vlSymsp->TOP.eof}
    , vbl_int{vlSymsp->TOP.vbl_int}
    , harddis_out{vlSymsp->TOP.harddis_out}
    , varbeamen_out{vlSymsp->TOP.varbeamen_out}
    , data_in{vlSymsp->TOP.data_in}
    , data_out{vlSymsp->TOP.data_out}
    , hpos{vlSymsp->TOP.hpos}
    , vpos{vlSymsp->TOP.vpos}
    , htotal_out{vlSymsp->TOP.htotal_out}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vagnus_beamcounter::Vagnus_beamcounter(const char* _vcname__)
    : Vagnus_beamcounter(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_beamcounter::~Vagnus_beamcounter() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_beamcounter___024root___eval_debug_assertions(Vagnus_beamcounter___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_beamcounter___024root___eval_static(Vagnus_beamcounter___024root* vlSelf);
void Vagnus_beamcounter___024root___eval_initial(Vagnus_beamcounter___024root* vlSelf);
void Vagnus_beamcounter___024root___eval_settle(Vagnus_beamcounter___024root* vlSelf);
void Vagnus_beamcounter___024root___eval(Vagnus_beamcounter___024root* vlSelf);

void Vagnus_beamcounter::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_beamcounter::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_beamcounter___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_beamcounter___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_beamcounter___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_beamcounter___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_beamcounter___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_beamcounter::eventsPending() { return false; }

uint64_t Vagnus_beamcounter::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_beamcounter::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_beamcounter___024root___eval_final(Vagnus_beamcounter___024root* vlSelf);

VL_ATTR_COLD void Vagnus_beamcounter::final() {
    Vagnus_beamcounter___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_beamcounter::hierName() const { return vlSymsp->name(); }
const char* Vagnus_beamcounter::modelName() const { return "Vagnus_beamcounter"; }
unsigned Vagnus_beamcounter::threads() const { return 1; }
void Vagnus_beamcounter::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_beamcounter::atClone() const {
    contextp()->threadPoolpOnClone();
}
