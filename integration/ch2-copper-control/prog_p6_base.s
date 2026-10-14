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
;   P6_MUT_SWAP02  -- the values of C0 and C2 exchanged: the colour that
;                     comes first on the line is no longer red
;   P6_MUT_SWAP13  -- the values of C1 and C3 exchanged: the colour that
;                     appears lower on the screen is no longer yellow

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
        ifd     P6_MUT_C2
        ; LACE with one bitplane fetched from an all-zero area (COLOR00
        ; still shows everywhere), the interlaced one-plane display setup of
        ; SPECIFICATION.md P11.
        move.w  #$1204,BPLCON0(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #40,BPL1MOD(a0)
        move.l  #DATA2,BPL1PTH(a0)
        else
        move.w  #$0200,BPLCON0(a0)
        endif
        endif

        ; step 3: List O at LIST1. C0-C3 = $0F00,$00F0,$000F,$0FF0.
        ifd     P6_MUT_SWAP02
        move.l  #$0180000F,LIST1          ; mutation: the first MOVE sets C2's value
        else
        move.l  #$01800F00,LIST1          ; MOVE COLOR00,C0
        endif
        move.l  #$6061FFFE,LIST1+4        ; WAIT1: unchanged by any case
        ifd     P6_MUT_SWAP13
        move.l  #$01800FF0,LIST1+8        ; mutation: the second MOVE sets C3's value
        else
        move.l  #$018000F0,LIST1+8        ; MOVE COLOR00,C1
        endif
        ifd     P6_WAIT2_C1
        move.l  #$60C1FFFE,LIST1+12       ; contrast (i): second WAIT $60A1 -> $60C1
        else
        move.l  #$60A1FFFE,LIST1+12
        endif
        ifd     P6_MUT_SWAP02
        move.l  #$01800F00,LIST1+16       ; mutation: the third MOVE sets C0's value
        else
        move.l  #$0180000F,LIST1+16       ; MOVE COLOR00,C2
        endif
        ifd     P6_WAIT3_B0
        move.l  #$B061FFFE,LIST1+20       ; contrast (ii): third WAIT $A061 -> $B061
        else
        move.l  #$A061FFFE,LIST1+20
        endif
        ifd     P6_MUT_A
        move.l  #$FFFFFFFE,LIST1+24       ; precondition (a) mutation: END here
        move.l  #$01800FF0,LIST1+28       ; C3's MOVE placed after END: dead code
        else
        ifd     P6_MUT_SWAP13
        move.l  #$018000F0,LIST1+24       ; mutation: the fourth MOVE sets C1's value
        else
        move.l  #$01800FF0,LIST1+24       ; MOVE COLOR00,C3
        endif
        move.l  #$FFFFFFFE,LIST1+28       ; END
        endif

        ; step 4: start per 2.8.
        move.l  #LIST1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        ifd     P6_MUT_B
        move.w  #$8200,DMACON(a0)         ; precondition (b) mutation: COPEN clear
        else
        ifd     P6_MUT_C2
        move.w  #$8380,DMACON(a0)         ; COPEN and BPLEN
        else
        move.w  #$8280,DMACON(a0)
        endif
        endif
        move.w  #0,COPJMP1(a0)            ; strobe again (2.8 step 3)
        move.w  #3,MARKBUF+4              ; MJ

        ; step 5: sampler until four frame wraps have passed after t(MJ).
        ; None of these records is graded (B1-B3 do not apply); they only
        ; time the run so the video capture spans enough frames.
        lea     RECBUF,a6
        moveq   #0,d7
        ifd     P6_MUT_C3
        ; frames of different length after the enable: about a frame and a
        ; half after MJ, BEAMCON0 is written with its PAL bit clear.
        move.w  #20000,d6
.dly:   dbra    d6,.dly
        move.w  #$0000,$1DC(a0)
        endif
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
