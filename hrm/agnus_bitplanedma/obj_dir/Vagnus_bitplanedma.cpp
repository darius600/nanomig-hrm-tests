// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Model implementation (design independent parts)

#include "Vagnus_bitplanedma__pch.h"
#include "verilated_vcd_c.h"

//============================================================
// Constructors

Vagnus_bitplanedma::Vagnus_bitplanedma(VerilatedContext* _vcontextp__, const char* _vcname__)
    : VerilatedModel{*_vcontextp__}
    , vlSymsp{new Vagnus_bitplanedma__Syms(contextp(), _vcname__, this)}
    , clk{vlSymsp->TOP.clk}
    , clk7_en{vlSymsp->TOP.clk7_en}
    , reset{vlSymsp->TOP.reset}
    , harddis{vlSymsp->TOP.harddis}
    , aga{vlSymsp->TOP.aga}
    , ecs{vlSymsp->TOP.ecs}
    , a1k{vlSymsp->TOP.a1k}
    , sof{vlSymsp->TOP.sof}
    , dmaena{vlSymsp->TOP.dmaena}
    , hde{vlSymsp->TOP.hde}
    , dma{vlSymsp->TOP.dma}
    , reg_address_in{vlSymsp->TOP.reg_address_in}
    , reg_address_out{vlSymsp->TOP.reg_address_out}
    , vpos{vlSymsp->TOP.vpos}
    , hpos{vlSymsp->TOP.hpos}
    , data_in{vlSymsp->TOP.data_in}
    , address_out{vlSymsp->TOP.address_out}
    , rootp{&(vlSymsp->TOP)}
{
    // Register model with the context
    contextp()->addModel(this);
    contextp()->traceBaseModelCbAdd(
        [this](VerilatedTraceBaseC* tfp, int levels, int options) { traceBaseModel(tfp, levels, options); });
}

Vagnus_bitplanedma::Vagnus_bitplanedma(const char* _vcname__)
    : Vagnus_bitplanedma(Verilated::threadContextp(), _vcname__)
{
}

//============================================================
// Destructor

Vagnus_bitplanedma::~Vagnus_bitplanedma() {
    delete vlSymsp;
}

//============================================================
// Evaluation function

#ifdef VL_DEBUG
void Vagnus_bitplanedma___024root___eval_debug_assertions(Vagnus_bitplanedma___024root* vlSelf);
#endif  // VL_DEBUG
void Vagnus_bitplanedma___024root___eval_static(Vagnus_bitplanedma___024root* vlSelf);
void Vagnus_bitplanedma___024root___eval_initial(Vagnus_bitplanedma___024root* vlSelf);
void Vagnus_bitplanedma___024root___eval_settle(Vagnus_bitplanedma___024root* vlSelf);
void Vagnus_bitplanedma___024root___eval(Vagnus_bitplanedma___024root* vlSelf);

void Vagnus_bitplanedma::eval_step() {
    VL_DEBUG_IF(VL_DBG_MSGF("+++++TOP Evaluate Vagnus_bitplanedma::eval_step\n"); );
#ifdef VL_DEBUG
    // Debug assertions
    Vagnus_bitplanedma___024root___eval_debug_assertions(&(vlSymsp->TOP));
#endif  // VL_DEBUG
    vlSymsp->__Vm_activity = true;
    vlSymsp->__Vm_deleter.deleteAll();
    if (VL_UNLIKELY(!vlSymsp->__Vm_didInit)) {
        vlSymsp->__Vm_didInit = true;
        VL_DEBUG_IF(VL_DBG_MSGF("+ Initial\n"););
        Vagnus_bitplanedma___024root___eval_static(&(vlSymsp->TOP));
        Vagnus_bitplanedma___024root___eval_initial(&(vlSymsp->TOP));
        Vagnus_bitplanedma___024root___eval_settle(&(vlSymsp->TOP));
    }
    VL_DEBUG_IF(VL_DBG_MSGF("+ Eval\n"););
    Vagnus_bitplanedma___024root___eval(&(vlSymsp->TOP));
    // Evaluate cleanup
    Verilated::endOfEval(vlSymsp->__Vm_evalMsgQp);
}

//============================================================
// Events and timing
bool Vagnus_bitplanedma::eventsPending() { return false; }

uint64_t Vagnus_bitplanedma::nextTimeSlot() {
    VL_FATAL_MT(__FILE__, __LINE__, "", "No delays in the design");
    return 0;
}

//============================================================
// Utilities

const char* Vagnus_bitplanedma::name() const {
    return vlSymsp->name();
}

//============================================================
// Invoke final blocks

void Vagnus_bitplanedma___024root___eval_final(Vagnus_bitplanedma___024root* vlSelf);

VL_ATTR_COLD void Vagnus_bitplanedma::final() {
    Vagnus_bitplanedma___024root___eval_final(&(vlSymsp->TOP));
}

//============================================================
// Implementations of abstract methods from VerilatedModel

const char* Vagnus_bitplanedma::hierName() const { return vlSymsp->name(); }
const char* Vagnus_bitplanedma::modelName() const { return "Vagnus_bitplanedma"; }
unsigned Vagnus_bitplanedma::threads() const { return 1; }
void Vagnus_bitplanedma::prepareClone() const { contextp()->prepareClone(); }
void Vagnus_bitplanedma::atClone() const {
    contextp()->threadPoolpOnClone();
}
std::unique_ptr<VerilatedTraceConfig> Vagnus_bitplanedma::traceConfig() const {
    return std::unique_ptr<VerilatedTraceConfig>{new VerilatedTraceConfig{false, false, false}};
};

//============================================================
// Trace configuration

void Vagnus_bitplanedma___024root__trace_decl_types(VerilatedVcd* tracep);

void Vagnus_bitplanedma___024root__trace_init_top(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD static void trace_init(void* voidSelf, VerilatedVcd* tracep, uint32_t code) {
    // Callback from tracep->open()
    Vagnus_bitplanedma___024root* const __restrict vlSelf VL_ATTR_UNUSED = static_cast<Vagnus_bitplanedma___024root*>(voidSelf);
    Vagnus_bitplanedma__Syms* const __restrict vlSymsp VL_ATTR_UNUSED = vlSelf->vlSymsp;
    if (!vlSymsp->_vm_contextp__->calcUnusedSigs()) {
        VL_FATAL_MT(__FILE__, __LINE__, __FILE__,
            "Turning on wave traces requires Verilated::traceEverOn(true) call before time 0.");
    }
    vlSymsp->__Vm_baseCode = code;
    tracep->pushPrefix(std::string{vlSymsp->name()}, VerilatedTracePrefixType::SCOPE_MODULE);
    Vagnus_bitplanedma___024root__trace_decl_types(tracep);
    Vagnus_bitplanedma___024root__trace_init_top(vlSelf, tracep);
    tracep->popPrefix();
}

VL_ATTR_COLD void Vagnus_bitplanedma___024root__trace_register(Vagnus_bitplanedma___024root* vlSelf, VerilatedVcd* tracep);

VL_ATTR_COLD void Vagnus_bitplanedma::traceBaseModel(VerilatedTraceBaseC* tfp, int levels, int options) {
    (void)levels; (void)options;
    VerilatedVcdC* const stfp = dynamic_cast<VerilatedVcdC*>(tfp);
    if (VL_UNLIKELY(!stfp)) {
        vl_fatal(__FILE__, __LINE__, __FILE__,"'Vagnus_bitplanedma::trace()' called on non-VerilatedVcdC object;"
            " use --trace-fst with VerilatedFst object, and --trace-vcd with VerilatedVcd object");
    }
    stfp->spTrace()->addModel(this);
    stfp->spTrace()->addInitCb(&trace_init, &(vlSymsp->TOP));
    Vagnus_bitplanedma___024root__trace_register(&(vlSymsp->TOP), stfp->spTrace());
}
