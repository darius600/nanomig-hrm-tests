; prog_cb_eD_cpublt80.s -- prog_cb.s on ECS with COPCON=$FFFD, CPU BLTSIZE write at V>=$80
IS_ECS          equ     1
CV              equ     $FFFD
MUT_CPUBLT80    equ     1
        include "prog_cb.s"
