; prog_rg_pre48.s -- R_G with the configuring writes placed before its first
; read of $00E80000 (GATE-G-PRESENCE's mutation): the byte writes to registers
; 4A and 48 come first, the reads of the 32 words are unchanged. Writing
; register 48 configures the board (p.436) and the configuration registers
; move out of $00E8xxxx (p.435 footnote 6), so no unconfigured PIC is left for
; the reads.

MUT_PRE48 equ 1
        include "prog_rg.s"
