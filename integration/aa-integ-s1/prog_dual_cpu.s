; prog_dual_cpu.s -- P14's run U3: the register list, then 1024 samples, each "read
; HHPOSR ($DFF1DA), store; read VHPOSR, store" into SBUF, one after the other,
; then DONE. Only the largest VHPOSR bits 7-0 of the samples is used, so the
; sample times are not paced. prog_dual.s waits for the main counter to reach a
; target before every sample, which a counter whose lines are shorter than the
; targets (run U3) never allows.
        include "regs.i"
SBUF    equ     $0C8000
HHPOSR  equ     $1DA
        lea     SBUF,a2
        move.w  #1023,d7
.sl:    move.w  HHPOSR(a0),(a2)+
        move.w  VHPOSR(a0),(a2)+
        dbra    d7,.sl
        DONE
