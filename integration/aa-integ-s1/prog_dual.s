; prog_dual.s -- P14: the register list, then 1024 samples, each "read HHPOSR
; ($DFF1DA), store; read VHPOSR, store" into SBUF, then DONE.
;
; The sample times are set by the machine's main beam counter, not by the CPU's
; instruction timing. A target T, a byte, advances by a step of 38 to 44 after every
; sample. A sample is taken at the first poll of VHPOSR bits 7-0 (the main
; counter's H8-H1) at which (m - T) mod 256 < 128: once the counter has reached
; T, or, with T beyond the end of the line, once it has restarted. A CPU that
; runs the loop faster or slower reaches the same targets one poll later or
; earlier; it does not change which positions of which lines are sampled. The 16
; steps of an unrolled block add up to 657, an odd number, so T takes 256
; different values at the first sample of 256 successive blocks.
;
; Every second sample has a NOP more in its polling loop and a NOP more after its
; stores, so that the polls of different samples do not all lie on one lattice
; of the CPU's instruction timing. PAD, defined before the include of this file
; by prog_dual_pad121.s, adds NOPs after each sample.
        include "regs.i"
SBUF    equ     $0C8000
HHPOSR  equ     $1DA

SAMPLE  macro                           ; \1 = step of T, \2 = NOPs in the polling loop, \3 = NOPs after the stores
.p\@:   move.w  VHPOSR(a0),d1           ; m, with V7-V0 above it
        sub.b   d0,d1                   ; m - T, modulo 256
        rept    \2
        nop
        endr
        bmi.s   .p\@                    ; (m - T) mod 256 >= 128: T not reached
        move.w  HHPOSR(a0),(a2)+
        move.w  VHPOSR(a0),(a2)+
        rept    \3
        nop
        endr
        add.b   #\1,d0
        ifd     PAD
        rept    PAD
        nop
        endr
        endc
        endm

        lea     SBUF,a2
        move.w  #63,d7
        moveq   #0,d0                   ; T
.blk:   SAMPLE  41,0,0
        SAMPLE  44,1,1
        SAMPLE  43,0,0
        SAMPLE  41,1,1
        SAMPLE  38,0,0
        SAMPLE  40,1,1
        SAMPLE  44,0,0
        SAMPLE  38,1,1
        SAMPLE  41,0,0
        SAMPLE  39,1,1
        SAMPLE  38,0,0
        SAMPLE  40,1,1
        SAMPLE  41,0,0
        SAMPLE  44,1,1
        SAMPLE  41,0,0
        SAMPLE  44,1,1
        dbra    d7,.blk
        DONE
