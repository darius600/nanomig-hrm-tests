; prog_p4_ctrl_cop1c_mut_a.s -- P4 comparator contrast C, precondition (a) mutation: DMACON=$8080 written before the DMACONR read. See prog_p4_base.s.
P4_CTRL_COP1C equ 1
P4_MUT_A equ 1
        include "prog_p4_base.s"
