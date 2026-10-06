; prog_regs_nofix.s -- prog_regs.s without the fixture store (the fixture's contrast).
NOFIX   equ     1
        include "regs.i"
.idle:  bra.s   .idle
