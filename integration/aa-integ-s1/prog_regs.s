; prog_regs.s -- the register list, then idle (P6-P9, P15 setups, unit calibration).
        include "regs.i"
.idle:  bra.s   .idle
