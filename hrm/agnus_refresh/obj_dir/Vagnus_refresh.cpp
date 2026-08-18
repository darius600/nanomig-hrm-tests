// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_refresh__pch.h"

//============================================================
// Constructors

Vagnus_refresh::Vagnus_refresh(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_refresh__Syms(contextp(), _vcname__, this)}
    , dma{vlSymsp->TOP.dma}
    , hpos{vlSymsp->TOP.hpos}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vagnus_refresh::Vagnus_refresh(const char* _vcname__)
    : Vagnus_refresh(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_refresh::~Vagnus_refresh() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_refresh___024root___eval_debug_assertions(Vagnus_refresh___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_refresh___024root___eval_static(Vagnus_refresh___024root* vlSelf);
void Vagnus_refresh___024root___eval_initial(Vagnus_refresh___024root* vlSelf);
void Vagnus_refresh___024root___eval_settle(Vagnus_refresh___024root* vlSelf);
void Vagnus_refresh___024root___eval(Vagnus_refresh___024root* vlSelf);

void Vagnus_refresh::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_refresh::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_refresh___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_refresh___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_refresh___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_refresh___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_refresh___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_refresh::eventsPending() { return false; }

uint64_t Vagnus_refresh::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_refresh::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_refresh___024root___eval_final(Vagnus_refresh___024root* vlSelf);

VL_ATTR_COLD void Vagnus_refresh::final() {
    Vagnus_refresh___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_refresh::hierName() const { return vlSymsp->name(); }
const char* Vagnus_refresh::modelName() const { return "Vagnus_refresh"; }
unsigned Vagnus_refresh::threads() const { return 1; }
void Vagnus_refresh::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_refresh::atClone() const {
    contextp()->threadPoolpOnClone();
}
