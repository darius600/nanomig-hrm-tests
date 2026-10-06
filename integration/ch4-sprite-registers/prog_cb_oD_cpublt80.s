; prog_cb_oD_cpublt80.s -- prog_cb.s on OCS with COPCON=$FFFD, CPU BLTSIZE write at V>=$80
CV              equ     $FFFD
MUT_CPUBLT80    equ     1
        include "prog_cb.s"
