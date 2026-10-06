; prog_cb_o2_noblt.s -- prog_cb.s on OCS with COPCON=$0002, DMACON without BLTEN
CV              equ     $0002
MUT_NOBLT       equ     1
        include "prog_cb.s"
