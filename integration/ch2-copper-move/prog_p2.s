; prog_p2.s -- P2: IR2 bits 15-1 reach COP1LCL and IR2 bits 2-0 reach COP1LCH;
; shown by the address where the Copper restarts (SPECIFICATION.md P2). The
; twelve segments are placed by the test.
;
; Symbols this source answers to:
;   P2_ATTR  a CPU read of the first segment before the Copper starts

        include "nanomig.i"
        include "ch2move.i"

        org     PROGRAM_ORG

start:
        BASELINE
        PARKPTRS
        ifd     P2_ATTR
        move.w  $34000,d1
        endc
        COPSTART $34000
        IDLEFIELDS 17
        DONE
