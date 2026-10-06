; prog_cb_e2_cpuwr.s -- prog_cb.s on ECS with COPCON=$0002, CPU write to $30200 inside M1-M2
IS_ECS          equ     1
CV              equ     $0002
MUT_CPUWR       equ     1
        include "prog_cb.s"
