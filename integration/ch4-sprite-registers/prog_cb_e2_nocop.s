; prog_cb_e2_nocop.s -- prog_cb.s on ECS with COPCON=$0002, DMACON without COPEN
IS_ECS          equ     1
CV              equ     $0002
MUT_NOCOP       equ     1
        include "prog_cb.s"
