; prog_cb_o2_cpuwr.s -- prog_cb.s on OCS with COPCON=$0002, CPU write to $30200 inside M1-M2
CV              equ     $0002
MUT_CPUWR       equ     1
        include "prog_cb.s"
