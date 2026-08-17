// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_blitter_mask__pch.h"

//============================================================
// Constructors

Vagnus_blitter_mask::Vagnus_blitter_mask(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_blitter_mask__Syms(contextp(), _vcname__, this)}
    , first_word{vlSymsp->TOP.first_word}
    , last_word{vlSymsp->TOP.last_word}
    , bltafwm{vlSymsp->TOP.bltafwm}
    , bltalwm{vlSymsp->TOP.bltalwm}
    , bltadat{vlSymsp->TOP.bltadat}
    , mask_out{vlSymsp->TOP.mask_out}
    , data_out{vlSymsp->TOP.data_out}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vagnus_blitter_mask::Vagnus_blitter_mask(const char* _vcname__)
    : Vagnus_blitter_mask(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_blitter_mask::~Vagnus_blitter_mask() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_blitter_mask___024root___eval_debug_assertions(Vagnus_blitter_mask___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_blitter_mask___024root___eval_static(Vagnus_blitter_mask___024root* vlSelf);
void Vagnus_blitter_mask___024root___eval_initial(Vagnus_blitter_mask___024root* vlSelf);
void Vagnus_blitter_mask___024root___eval_settle(Vagnus_blitter_mask___024root* vlSelf);
void Vagnus_blitter_mask___024root___eval(Vagnus_blitter_mask___024root* vlSelf);

void Vagnus_blitter_mask::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_blitter_mask::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_blitter_mask___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_blitter_mask___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_blitter_mask___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_blitter_mask___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_blitter_mask___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_blitter_mask::eventsPending() { return false; }

uint64_t Vagnus_blitter_mask::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_blitter_mask::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_blitter_mask___024root___eval_final(Vagnus_blitter_mask___024root* vlSelf);

VL_ATTR_COLD void Vagnus_blitter_mask::final() {
    Vagnus_blitter_mask___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_blitter_mask::hierName() const { return vlSymsp->name(); }
const char* Vagnus_blitter_mask::modelName() const { return "Vagnus_blitter_mask"; }
unsigned Vagnus_blitter_mask::threads() const { return 1; }
void Vagnus_blitter_mask::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_blitter_mask::atClone() const {
    contextp()->threadPoolpOnClone();
}
