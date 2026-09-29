; prog_p6_base.s -- P6: beam coordinates run left to right and top to
; bottom (SPECIFICATION.md P6; no claim id). Included by the case files
; below, each setting one symbol first (default = main run).
;
;   (no symbol)    -- main run
;   P6_WAIT2_C1    -- contrast (i): second WAIT $60A1 -> $60C1
;   P6_WAIT3_B0    -- contrast (ii): third WAIT $A061 -> $B061
;   P6_MUT_A       -- precondition (a) mutation: the C3 MOVE placed after
;                     the final WAIT (never executes)
;   P6_MUT_B       -- precondition (b) mutation: DMACON = $8200 (COPEN clear)
;   P6_MUT_C       -- precondition (c) mutation: LACE set (2.5's own V1 contrast)

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL_DDR
        lea     CUSTOM,a0
        CLEAR_DMA_INT

        ; step 2: no bitplanes, colour on (p.23); (c)'s mutation also sets LACE.
        ifd     P6_MUT_C
        move.w  #$0204,BPLCON0(a0)
        else
        move.w  #$0200,BPLCON0(a0)
        endif

        ; step 3: List O at LIST1. C0-C3 = $0F00,$00F0,$000F,$0FF0.
        move.l  #$01800F00,LIST1          ; MOVE COLOR00,C0
        move.l  #$6061FFFE,LIST1+4        ; WAIT1: unchanged by any case
        move.l  #$018000F0,LIST1+8        ; MOVE COLOR00,C1
        ifd     P6_WAIT2_C1
        move.l  #$60C1FFFE,LIST1+12       ; contrast (i): second WAIT $60A1 -> $60C1
        else
        move.l  #$60A1FFFE,LIST1+12
        endif
        move.l  #$0180000F,LIST1+16       ; MOVE COLOR00,C2
        ifd     P6_WAIT3_B0
        move.l  #$B061FFFE,LIST1+20       ; contrast (ii): third WAIT $A061 -> $B061
        else
        move.l  #$A061FFFE,LIST1+20
        endif
        ifd     P6_MUT_A
        move.l  #$FFFFFFFE,LIST1+24       ; precondition (a) mutation: END here
        move.l  #$01800FF0,LIST1+28       ; C3's MOVE placed after END: dead code
        else
        move.l  #$01800FF0,LIST1+24       ; MOVE COLOR00,C3
        move.l  #$FFFFFFFE,LIST1+28       ; END
        endif

        ; step 4: start per 2.8.
        move.l  #LIST1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        ifd     P6_MUT_B
        move.w  #$8200,DMACON(a0)         ; precondition (b) mutation: COPEN clear
        else
        move.w  #$8280,DMACON(a0)
        endif
        move.w  #0,COPJMP1(a0)            ; strobe again (2.8 step 3)
        move.w  #3,MARKBUF+4              ; MJ

        ; step 5: sampler until four frame wraps have passed after t(MJ).
        ; None of these records is graded (B1-B3 do not apply); they only
        ; time the run so the video capture spans enough frames.
        lea     RECBUF,a6
        moveq   #0,d7
        move.w  #3000-1,d3
.pd:    SAMPLE
        SAMPLE
        SAMPLE
        SAMPLE
        SAMPLE
        SAMPLE
        SAMPLE
        SAMPLE
        addq.l  #8,d7
        dbra    d3,.pd

        move.l  d7,RESULTS+8

        DONE
