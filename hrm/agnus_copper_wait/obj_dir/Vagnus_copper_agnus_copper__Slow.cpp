// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vagnus_copper.h for the primary calling header

#include "Vagnus_copper__pch.h"
#include "Vagnus_copper__Syms.h"
#include "Vagnus_copper_agnus_copper.h"

void Vagnus_copper_agnus_copper___ctor_var_reset(Vagnus_copper_agnus_copper* vlSelf);

Vagnus_copper_agnus_copper::Vagnus_copper_agnus_copper(Vagnus_copper__Syms* symsp, const char* v__name)
    : VerilatedModule{v__name}
    , vlSymsp{symsp}
 {
    // Reset structure values
    Vagnus_copper_agnus_copper___ctor_var_reset(this);
}

void Vagnus_copper_agnus_copper::__Vconfigure(bool first) {
    (void)first;  // Prevent unused variable warning
}

Vagnus_copper_agnus_copper::~Vagnus_copper_agnus_copper() {
}
