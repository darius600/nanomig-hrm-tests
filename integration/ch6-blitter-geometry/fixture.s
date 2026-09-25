; fixture.s -- A0: the fixture proves a known blit result, and BLTSIZE
; written last is what starts it (ch6-intro.c20, ch6-keypoints.c1).
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A), ASH=0
        move.w  #$0000,BLTCON1(a0)     ; ascending, BSH=0
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,SRC
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        ifnd    NOBLTSIZE
        move.w  #(1<<6)|4,BLTSIZE(a0)  ; height 1, width 4 words -- starts it
        WAITBLT
        endc
        DONE

        org     SRC
        dc.w    $1234,$5678,$9ABC,$DEF0
