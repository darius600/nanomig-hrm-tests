// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_blitter_zero__pch.h"

//============================================================
// Constructors

Vagnus_blitter_zero::Vagnus_blitter_zero(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_blitter_zero__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , clk7_en{vlSymsp->TOP.clk7_en}
    , enable{vlSymsp->TOP.enable}
    , init{vlSymsp->TOP.init}
    , store_result{vlSymsp->TOP.store_result}
    , zero{vlSymsp->TOP.zero}
    , fill_out{vlSymsp->TOP.fill_out}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vagnus_blitter_zero::Vagnus_blitter_zero(const char* _vcname__)
    : Vagnus_blitter_zero(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_blitter_zero::~Vagnus_blitter_zero() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_blitter_zero___024root___eval_debug_assertions(Vagnus_blitter_zero___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_blitter_zero___024root___eval_static(Vagnus_blitter_zero___024root* vlSelf);
void Vagnus_blitter_zero___024root___eval_initial(Vagnus_blitter_zero___024root* vlSelf);
void Vagnus_blitter_zero___024root___eval_settle(Vagnus_blitter_zero___024root* vlSelf);
void Vagnus_blitter_zero___024root___eval(Vagnus_blitter_zero___024root* vlSelf);

void Vagnus_blitter_zero::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_blitter_zero::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_blitter_zero___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_blitter_zero___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_blitter_zero___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_blitter_zero___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_blitter_zero___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_blitter_zero::eventsPending() { return false; }

uint64_t Vagnus_blitter_zero::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_blitter_zero::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_blitter_zero___024root___eval_final(Vagnus_blitter_zero___024root* vlSelf);

VL_ATTR_COLD void Vagnus_blitter_zero::final() {
    Vagnus_blitter_zero___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_blitter_zero::hierName() const { return vlSymsp->name(); }
const char* Vagnus_blitter_zero::modelName() const { return "Vagnus_blitter_zero"; }
unsigned Vagnus_blitter_zero::threads() const { return 1; }
void Vagnus_blitter_zero::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_blitter_zero::atClone() const {
    contextp()->threadPoolpOnClone();
}
