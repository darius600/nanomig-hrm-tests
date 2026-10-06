; prog_cpuuh.s -- P15's live input for the comparators: after the register list the CPU reads
; one word of the region U = $160000 every (26 + 4 * NOPS) CPU clocks and advances its
; pointer by 8 bytes, the pattern the UHRES pointer is graded against. NOPS is defined
; before the include of this file by prog_cpuuh_line.s and prog_cpuuh_57.s.
        include "regs.i"
        lea     $160000,a1
.l:     move.w  (a1),d0
        lea     8(a1),a1
        rept    NOPS
        nop
        endr
        bra.w   .l
