; rowmask.s -- A1: BLTAFWM/BLTALWM mask the first/last word of EVERY
; raster row, not just the first/last row of the whole blit
; (6-intro.c13, 6-intro.c14, ch6-intro.c12, ch6-intro.c13).
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A), ASH=0
        move.w  #$0000,BLTCON1(a0)     ; ascending, BSH=0
        ifnd    WRONGFWM
        move.w  #$00FF,BLTAFWM(a0)
        else
        move.w  #$0F0F,BLTAFWM(a0)     ; mutation: a different first-word mask
        endc
        move.w  #$FF00,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,SRC
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        move.w  #(2<<6)|3,BLTSIZE(a0)  ; height 2, width 3 words
        WAITBLT
        DONE

        org     SRC
        dc.w    $FFFF,$FFFF,$FFFF,$FFFF,$FFFF,$FFFF
