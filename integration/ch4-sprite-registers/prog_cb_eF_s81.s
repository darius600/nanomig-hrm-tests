; prog_cb_eF_s81.s -- prog_cb.s on ECS with COPCON=$FFFF, first BLTSIZE MOVE $0081
IS_ECS          equ     1
CV              equ     $FFFF
MUT_S81         equ     1
        include "prog_cb.s"
