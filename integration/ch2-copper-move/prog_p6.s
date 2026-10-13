; prog_p6.s -- P6: a 680x0 long-word write puts the most significant word at the
; lower word address (SPECIFICATION.md P6).
;
; Before marker 1 the program identifies the CPU setting and stores the result
; at RESULTS+4: MOVEC VBR,D0 is a 68010 instruction and EXTB.L D0 a 68020
; instruction; an instruction the CPU does not have raises the illegal
; instruction exception (vector 4), whose handler steps over it. The stored word
; is 0 for a 68000, 1 for a 68010 and 2 for a 68020.
;
; Symbols this source answers to:
;   P6_VALUE   the long word written (default $12345678)
;   P6_NOPRE   the two word writes before marker 1 are omitted
;   P6_PROBE2  the identification is replaced by the constant 2

        include "nanomig.i"
        include "ch2move.i"

        ifnd    P6_VALUE
P6_VALUE        equ     $12345678
        endc

        org     PROGRAM_ORG

start:
        BASELINE
        ifd     P6_PROBE2
        moveq   #2,d2
        else
        move.l  #illegal,$10
        moveq   #0,d2
        moveq   #0,d7
        dc.w    $4E7A,$0801             ; MOVEC VBR,D0
        tst.l   d7
        bne.s   .probed
        moveq   #1,d2
        moveq   #0,d7
        dc.w    $49C0                   ; EXTB.L D0
        tst.l   d7
        bne.s   .probed
        moveq   #2,d2
.probed:
        endc
        move.w  d2,RESULTS+4
        ifnd    P6_NOPRE
        move.w  #$AAAA,$46000
        move.w  #$5555,$46002
        endc
        MARK    1
        move.l  #P6_VALUE,$46000
        MARK    2
        move.w  $46000,RESULTS
        move.w  $46002,RESULTS+2
        DONE

; Illegal instruction handler: flags the exception in D7 and steps over the
; instruction (4 bytes for the first probe, 2 for the second).
illegal:
        moveq   #1,d7
        tst.w   d2
        bne.s   .second
        addq.l  #4,2(sp)
        rte
.second:
        addq.l  #2,2(sp)
        rte
