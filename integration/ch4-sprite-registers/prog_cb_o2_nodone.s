; prog_cb_o2_nodone.s -- prog_cb.s on OCS with COPCON=$0002, the program never writes DONE
CV              equ     $0002
MUT_NODONE      equ     1
        include "prog_cb.s"
