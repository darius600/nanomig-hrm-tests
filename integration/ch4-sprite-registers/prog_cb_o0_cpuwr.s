; prog_cb_o0_cpuwr.s -- prog_cb.s on OCS with COPCON=$0000, CPU write to $30200 inside M1-M2
CV              equ     $0000
MUT_CPUWR       equ     1
        include "prog_cb.s"
