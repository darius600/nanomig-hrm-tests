; prog_dual_sync.s -- prog_dual.s with one sample per line: before each sample the
; loop waits until the main counter's H8-H1 (VHPOSR bits 7-0) lies in $20-$2F.
        include "regs.i"
SBUF    equ     $0C8000
HHPOSR  equ     $1DA
        lea     SBUF,a2
        move.w  #1023,d7
.sl:    move.w  VHPOSR(a0),d0
        sub.b   #$20,d0
        cmp.b   #16,d0
        bcc.s   .sl
        move.w  HHPOSR(a0),(a2)+
        move.w  VHPOSR(a0),(a2)+
        dbra    d7,.sl
        DONE
