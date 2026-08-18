// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_audiodma__pch.h"

//============================================================
// Constructors

Vagnus_audiodma::Vagnus_audiodma(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_audiodma__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , clk7_en{vlSymsp->TOP.clk7_en}
    , dma{vlSymsp->TOP.dma}
    , audio_dmal{vlSymsp->TOP.audio_dmal}
    , audio_dmas{vlSymsp->TOP.audio_dmas}
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

Vagnus_audiodma::Vagnus_audiodma(const char* _vcname__)
    : Vagnus_audiodma(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_audiodma::~Vagnus_audiodma() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_audiodma___024root___eval_debug_assertions(Vagnus_audiodma___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_audiodma___024root___eval_static(Vagnus_audiodma___024root* vlSelf);
void Vagnus_audiodma___024root___eval_initial(Vagnus_audiodma___024root* vlSelf);
void Vagnus_audiodma___024root___eval_settle(Vagnus_audiodma___024root* vlSelf);
void Vagnus_audiodma___024root___eval(Vagnus_audiodma___024root* vlSelf);

void Vagnus_audiodma::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_audiodma::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_audiodma___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_audiodma___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_audiodma___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_audiodma___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_audiodma___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_audiodma::eventsPending() { return false; }

uint64_t Vagnus_audiodma::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_audiodma::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_audiodma___024root___eval_final(Vagnus_audiodma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_audiodma::final() {
    Vagnus_audiodma___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_audiodma::hierName() const { return vlSymsp->name(); }
const char* Vagnus_audiodma::modelName() const { return "Vagnus_audiodma"; }
unsigned Vagnus_audiodma::threads() const { return 1; }
void Vagnus_audiodma::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_audiodma::atClone() const {
    contextp()->threadPoolpOnClone();
}
