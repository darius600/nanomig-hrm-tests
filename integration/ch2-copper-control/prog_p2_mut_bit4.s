; prog_p2_mut_bit4.s -- P2 clause contrast (data placement): phase 2 writes
; DMACON = $0080 (no fetch) and the program sets INTREQ bit 4 itself after
; M2. See prog_p2_base.s.
P2_MUT_BIT4     equ     1
        include "prog_p2_base.s"
