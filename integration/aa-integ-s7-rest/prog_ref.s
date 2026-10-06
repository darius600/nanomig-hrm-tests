; prog_ref.s -- P1 and P2 (SPECIFICATION.md section 5): a Copper MOVE to the
; register REF_DA with the data REF_VAL, COPCON = REF_COPCON.
; A wrapper sets REF_VAL, REF_DA and REF_COPCON before including this file.
; Optional symbols: REF_NOP (one NOP right after the vertical-blank wait),
; REF_ENDLIST (the end WAIT is listed at L+0), NOEND.

        include "s7r.i"

        ifnd    REF_DA
REF_DA          equ     $0028
        endc
        ifnd    REF_COPCON
REF_COPCON      equ     $0002
        endc

        org     PROGRAM_ORG

start:
        PROLOGUE
        ; step 2: the Copper list at L
        ifd     REF_ENDLIST
        move.w  #$FFFF,L
        move.w  #$FFFE,L+2
        else
        move.w  #$4001,L
        move.w  #$FFFE,L+2
        endc
        move.w  #REF_DA,L+4
        move.w  #REF_VAL,L+6
        move.w  #$FFFF,L+8
        move.w  #$FFFE,L+10
        VBWAIT
        ifd     REF_NOP
        nop
        endc
        move.w  #(L>>16),COP1LCH(a0)
        move.w  #(L&$FFFF),COP1LCL(a0)
        move.w  d0,COPJMP1(a0)
        move.w  #REF_COPCON,COPCON(a0)
        move.w  #$8280,DMACON(a0)
.wait:  move.w  VPOSR(a0),d1
        and.w   #7,d1
        bne.s   .line
        move.w  VHPOSR(a0),d0
        lsr.w   #8,d0
        cmp.b   #$60,d0
        blo.s   .wait
.line:  EPILOGUE
