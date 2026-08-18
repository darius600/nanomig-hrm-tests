// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_copper.h for the primary calling header

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper__Syms.h"
#include "Vagnus_copper___024root.h"

void Vagnus_copper___024root___ctor_var_reset(Vagnus_copper___024root* vlSelf);

Vagnus_copper___024root::Vagnus_copper___024root(Vagnus_copper__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vagnus_copper___024root___ctor_var_reset(this);
}

void Vagnus_copper___024root::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vagnus_copper___024root::~Vagnus_copper___024root() {
}
