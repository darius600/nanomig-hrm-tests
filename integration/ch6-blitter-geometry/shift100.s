; shift100.s -- A6: a shift over 15 bits is a whole-word destination-
; pointer displacement plus a residual barrel shift. Worked example: a
; 100-bit right shift is a 6-word (12-byte) pointer advance plus a 4-bit
; residual shift (6-shifts-and-masks.c10/c11/c12/c13, ch6-shifts.c10/c11/
; c12).
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$49F0,BLTCON0(a0)     ; ASH=4, USEA+USED, LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)     ; ascending
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,SRC
        ifnd    WRONGADVANCE
        SETPTR  BLTDPTH,BLTDPTL,DST+12 ; 100/16 = 6 words = 12 bytes
        else
        SETPTR  BLTDPTH,BLTDPTL,DST+10 ; mutation: 5 words instead of 6
        endc
        BLTEN_ON
        move.w  #(1<<6)|3,BLTSIZE(a0)  ; height 1, width 3 words
        WAITBLT
        DONE

        org     SRC
        dc.w    $1111,$2222,$3333
