// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus.h for the primary calling header

#include "Vagnus__pch.h"
#include "Vagnus__Syms.h"
#include "Vagnus___024root.h"

void Vagnus___024root___ctor_var_reset(Vagnus___024root* vlSelf);

Vagnus___024root::Vagnus___024root(Vagnus__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vagnus___024root___ctor_var_reset(this);
}

void Vagnus___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vagnus___024root::~Vagnus___024root() {
}
