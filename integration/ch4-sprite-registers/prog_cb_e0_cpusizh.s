; prog_cb_e0_cpusizh.s -- prog_cb.s on ECS with COPCON=$0000, CPU BLTSIZH write before M1
IS_ECS          equ     1
CV              equ     $0000
MUT_CPUSIZH     equ     1
        include "prog_cb.s"
