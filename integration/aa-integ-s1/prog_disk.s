; prog_disk.s -- P13: the register list (DSKPTH/DSKPTL, INTREQ, DMACON, DSKLEN
; $4000, $C008, $C008), then poll INTREQR bit 1 (DSKBLK, AA p.25) storing each
; read to RESULTS+2, and write DSKLEN = $4000 when it reads 1.
        include "regs.i"
DSKLEN  equ     $024
.pd:    move.w  INTREQR(a0),d0
        move.w  d0,RESULTS+2
        btst    #1,d0
        beq.s   .pd
        move.w  #$4000,DSKLEN(a0)
.idle:  bra.s   .idle
