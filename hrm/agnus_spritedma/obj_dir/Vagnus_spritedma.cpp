// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_spritedma__pch.h"

//============================================================
// Constructors

Vagnus_spritedma::Vagnus_spritedma(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_spritedma__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , clk7_en{vlSymsp->TOP.clk7_en}
    , reset{vlSymsp->TOP.reset}
    , aga{vlSymsp->TOP.aga}
    , ecs{vlSymsp->TOP.ecs}
    , reqdma{vlSymsp->TOP.reqdma}
    , ackdma{vlSymsp->TOP.ackdma}
    , vbl{vlSymsp->TOP.vbl}
    , vblend{vlSymsp->TOP.vblend}
    , reg_address_in{vlSymsp->TOP.reg_address_in}
    , reg_address_out{vlSymsp->TOP.reg_address_out}
    , hpos{vlSymsp->TOP.hpos}
    , vpos{vlSymsp->TOP.vpos}
    , data_in{vlSymsp->TOP.data_in}
    , address_out{vlSymsp->TOP.address_out}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vagnus_spritedma::Vagnus_spritedma(const char* _vcname__)
    : Vagnus_spritedma(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_spritedma::~Vagnus_spritedma() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_spritedma___024root___eval_debug_assertions(Vagnus_spritedma___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_spritedma___024root___eval_static(Vagnus_spritedma___024root* vlSelf);
void Vagnus_spritedma___024root___eval_initial(Vagnus_spritedma___024root* vlSelf);
void Vagnus_spritedma___024root___eval_settle(Vagnus_spritedma___024root* vlSelf);
void Vagnus_spritedma___024root___eval(Vagnus_spritedma___024root* vlSelf);

void Vagnus_spritedma::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_spritedma::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_spritedma___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_spritedma___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_spritedma___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_spritedma___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_spritedma___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_spritedma::eventsPending() { return false; }

uint64_t Vagnus_spritedma::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_spritedma::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_spritedma___024root___eval_final(Vagnus_spritedma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_spritedma::final() {
    Vagnus_spritedma___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_spritedma::hierName() const { return vlSymsp->name(); }
const char* Vagnus_spritedma::modelName() const { return "Vagnus_spritedma"; }
unsigned Vagnus_spritedma::threads() const { return 1; }
void Vagnus_spritedma::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_spritedma::atClone() const {
    contextp()->threadPoolpOnClone();
}
