; shiftdir_asc.s -- ch6-copy.c7/c8: when shifting, ascending mode shifts
; right; descending mode shifts left. Source 1111 2222 3333, ASH=4.
; p.182: in descending mode "shifts are then towards the left, rather
; than the right"; p.200 gives the same rule for both modes.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$49F0,BLTCON0(a0)     ; ASH=4, USEA+USED, LF=$F0 (D=A)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        ifnd    DESCMODE
        move.w  #$0000,BLTCON1(a0)             ; ascending: shift right
        SETPTR  BLTAPTH,BLTAPTL,SRC
        SETPTR  BLTDPTH,BLTDPTL,DST
        else
        move.w  #$0002,BLTCON1(a0)             ; descending: shift left
        SETPTR  BLTAPTH,BLTAPTL,SRC+4
        SETPTR  BLTDPTH,BLTDPTL,DST+4
        endc
        BLTEN_ON
        move.w  #(1<<6)|3,BLTSIZE(a0)
        WAITBLT
        DONE

        org     SRC
        dc.w    $1111,$2222,$3333
