; prog_cb_e2_noblt.s -- prog_cb.s on ECS with COPCON=$0002, DMACON without BLTEN
IS_ECS          equ     1
CV              equ     $0002
MUT_NOBLT       equ     1
        include "prog_cb.s"
