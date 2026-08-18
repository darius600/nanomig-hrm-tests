// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_diskdma__pch.h"

//============================================================
// Constructors

Vagnus_diskdma::Vagnus_diskdma(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_diskdma__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , clk7_en{vlSymsp->TOP.clk7_en}
    , dma{vlSymsp->TOP.dma}
    , dmal{vlSymsp->TOP.dmal}
    , dmas{vlSymsp->TOP.dmas}
    , speed{vlSymsp->TOP.speed}
    , wr{vlSymsp->TOP.wr}
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

Vagnus_diskdma::Vagnus_diskdma(const char* _vcname__)
    : Vagnus_diskdma(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_diskdma::~Vagnus_diskdma() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_diskdma___024root___eval_debug_assertions(Vagnus_diskdma___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_diskdma___024root___eval_static(Vagnus_diskdma___024root* vlSelf);
void Vagnus_diskdma___024root___eval_initial(Vagnus_diskdma___024root* vlSelf);
void Vagnus_diskdma___024root___eval_settle(Vagnus_diskdma___024root* vlSelf);
void Vagnus_diskdma___024root___eval(Vagnus_diskdma___024root* vlSelf);

void Vagnus_diskdma::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_diskdma::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_diskdma___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_diskdma___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_diskdma___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_diskdma___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_diskdma___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_diskdma::eventsPending() { return false; }

uint64_t Vagnus_diskdma::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_diskdma::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_diskdma___024root___eval_final(Vagnus_diskdma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_diskdma::final() {
    Vagnus_diskdma___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_diskdma::hierName() const { return vlSymsp->name(); }
const char* Vagnus_diskdma::modelName() const { return "Vagnus_diskdma"; }
unsigned Vagnus_diskdma::threads() const { return 1; }
void Vagnus_diskdma::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_diskdma::atClone() const {
    contextp()->threadPoolpOnClone();
}
