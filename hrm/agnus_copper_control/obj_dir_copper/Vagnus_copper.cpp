// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_copper__pch.h"

//============================================================
// Constructors

Vagnus_copper::Vagnus_copper(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_copper__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , clk7_en{vlSymsp->TOP.clk7_en}
    , reset{vlSymsp->TOP.reset}
    , ecs{vlSymsp->TOP.ecs}
    , reqdma{vlSymsp->TOP.reqdma}
    , ackdma{vlSymsp->TOP.ackdma}
    , enadma{vlSymsp->TOP.enadma}
    , sof{vlSymsp->TOP.sof}
    , blit_busy{vlSymsp->TOP.blit_busy}
    , vpos{vlSymsp->TOP.vpos}
    , reg_address_in{vlSymsp->TOP.reg_address_in}
    , reg_address_out{vlSymsp->TOP.reg_address_out}
    , hpos{vlSymsp->TOP.hpos}
    , data_in{vlSymsp->TOP.data_in}
    , address_out{vlSymsp->TOP.address_out}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vagnus_copper::Vagnus_copper(const char* _vcname__)
    : Vagnus_copper(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_copper::~Vagnus_copper() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_copper___024root___eval_debug_assertions(Vagnus_copper___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_copper___024root___eval_static(Vagnus_copper___024root* vlSelf);
void Vagnus_copper___024root___eval_initial(Vagnus_copper___024root* vlSelf);
void Vagnus_copper___024root___eval_settle(Vagnus_copper___024root* vlSelf);
void Vagnus_copper___024root___eval(Vagnus_copper___024root* vlSelf);

void Vagnus_copper::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_copper::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_copper___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_copper___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_copper___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_copper___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_copper___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_copper::eventsPending() { return false; }

uint64_t Vagnus_copper::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_copper::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_copper___024root___eval_final(Vagnus_copper___024root* vlSelf);

VL_ATTR_COLD void Vagnus_copper::final() {
    Vagnus_copper___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_copper::hierName() const { return vlSymsp->name(); }
const char* Vagnus_copper::modelName() const { return "Vagnus_copper"; }
unsigned Vagnus_copper::threads() const { return 1; }
void Vagnus_copper::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_copper::atClone() const {
    contextp()->threadPoolpOnClone();
}
