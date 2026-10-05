; prog_p4_ctrl_cop1c_mut_c.s -- P4 comparator contrast C, precondition (c) mutation: wait for a frame wrap between M1 and the DMACON write. See prog_p4_base.s.
P4_CTRL_COP1C equ 1
P4_MUT_C equ 1
        include "prog_p4_base.s"
