// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_blitter_line_octant__pch.h"

//============================================================
// Constructors

Vagnus_blitter_line_octant::Vagnus_blitter_line_octant(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_blitter_line_octant__Syms(contextp(), _vcname__, this)}
    , bltcon1{vlSymsp->TOP.bltcon1}
    , sign_del{vlSymsp->TOP.sign_del}
    , ash{vlSymsp->TOP.ash}
    , lineinc{vlSymsp->TOP.lineinc}
    , linedec{vlSymsp->TOP.linedec}
    , lineadd{vlSymsp->TOP.lineadd}
    , linesub{vlSymsp->TOP.linesub}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
}

Vagnus_blitter_line_octant::Vagnus_blitter_line_octant(const char* _vcname__)
    : Vagnus_blitter_line_octant(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_blitter_line_octant::~Vagnus_blitter_line_octant() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_blitter_line_octant___024root___eval_debug_assertions(Vagnus_blitter_line_octant___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_blitter_line_octant___024root___eval_static(Vagnus_blitter_line_octant___024root* vlSelf);
void Vagnus_blitter_line_octant___024root___eval_initial(Vagnus_blitter_line_octant___024root* vlSelf);
void Vagnus_blitter_line_octant___024root___eval_settle(Vagnus_blitter_line_octant___024root* vlSelf);
void Vagnus_blitter_line_octant___024root___eval(Vagnus_blitter_line_octant___024root* vlSelf);

void Vagnus_blitter_line_octant::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_blitter_line_octant::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_blitter_line_octant___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_blitter_line_octant___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_blitter_line_octant___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_blitter_line_octant___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_blitter_line_octant___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_blitter_line_octant::eventsPending() { return false; }

uint64_t Vagnus_blitter_line_octant::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_blitter_line_octant::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_blitter_line_octant___024root___eval_final(Vagnus_blitter_line_octant___024root* vlSelf);

VL_ATTR_COLD void Vagnus_blitter_line_octant::final() {
    Vagnus_blitter_line_octant___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_blitter_line_octant::hierName() const { return vlSymsp->name(); }
const char* Vagnus_blitter_line_octant::modelName() const { return "Vagnus_blitter_line_octant"; }
unsigned Vagnus_blitter_line_octant::threads() const { return 1; }
void Vagnus_blitter_line_octant::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_blitter_line_octant::atClone() const {
    contextp()->threadPoolpOnClone();
}
