; prog_dual.s -- P14: the register list, then 1024 samples, each "read HHPOSR
; ($DFF1DA), store; read VHPOSR, store" into SBUF, then DONE. The loop body is
; 42 CPU clocks (21 CCK) when no DMA slot is taken. PAD, defined before the
; include of this file by prog_dual_pad121.s, adds NOPs that make
; the body 242 clocks (121 CCK).
        include "regs.i"
SBUF    equ     $0C8000
HHPOSR  equ     $1DA
        lea     SBUF,a2
        move.w  #1023,d7
.sl:    move.w  HHPOSR(a0),(a2)+
        move.w  VHPOSR(a0),(a2)+
        ifd     PAD
        rept    PAD
        nop
        endr
        endc
        dbra    d7,.sl
        DONE
