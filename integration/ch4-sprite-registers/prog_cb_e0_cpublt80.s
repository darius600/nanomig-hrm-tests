; prog_cb_e0_cpublt80.s -- prog_cb.s on ECS with COPCON=$0000, CPU BLTSIZE write at V>=$80
IS_ECS          equ     1
CV              equ     $0000
MUT_CPUBLT80    equ     1
        include "prog_cb.s"
