; rastercont.s -- A2: with modulo 0, the destination word addresses form
; one unbroken "plus one" run with no gap at any row boundary
; (ch6-dma.c30, ch6-dma.c31). 7 words/row x 3 rows, echoing Figure 6-1's
; 7-column grid.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)     ; ascending
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        ifnd    ROWGAP
        move.w  #0,BLTDMOD(a0)
        else
        move.w  #4,BLTDMOD(a0)         ; mutation: a 2-word gap after each row
        endc
        SETPTR  BLTAPTH,BLTAPTL,SRC
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        move.w  #(3<<6)|7,BLTSIZE(a0)  ; height 3, width 7 words
        WAITBLT
        DONE

        org     SRC
        dc.w    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21
