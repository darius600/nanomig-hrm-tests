; prog_cb_eF_b0.s -- prog_cb.s on ECS in the $FFFF slot with COPCON=$FFFD (bit 1 clear)
IS_ECS          equ     1
CV              equ     $FFFD
        include "prog_cb.s"
