; prog_cb_e2_cpuearly.s -- prog_cb.s on ECS with COPCON=$0002, CPU BLTSIZE write before M1
IS_ECS          equ     1
CV              equ     $0002
MUT_CPUEARLY    equ     1
        include "prog_cb.s"
