; mintper_n.s -- ch6-venn.c2: minterm n is bit n of the LF byte, for every
; n = 0..7 individually (not just n=7 alone and 0-3 as a group). A=FF00,
; B=F0F0, C=CCCC, one blit per n, LF = 1<<n, results at DST+0..DST+14 for
; n = 7,6,5,4,3,2,1,0.
        include "nanomig.i"
        include "geom.i"

BITBLIT         macro
                move.w  #$0F00|(\1),BLTCON0(a0)
                SETPTR  BLTAPTH,BLTAPTL,MASKBUF
                SETPTR  BLTBPTH,BLTBPTL,SRC
                SETPTR  BLTCPTH,BLTCPTL,SRC2
                SETPTR  BLTDPTH,BLTDPTL,\2
                move.w  #(1<<6)|1,BLTSIZE(a0)
                WAITBLT
                endm

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        BLTEN_ON

        BITBLIT $80,DST+0        ; n=7
        BITBLIT $40,DST+2        ; n=6
        BITBLIT $20,DST+4        ; n=5
        ifnd    SWAPBIT4
        BITBLIT $10,DST+6        ; n=4
        else
        BITBLIT $08,DST+6        ; mutation: n=3's bit where n=4's belongs
        endc
        BITBLIT $08,DST+8        ; n=3
        BITBLIT $04,DST+10       ; n=2
        BITBLIT $02,DST+12       ; n=1
        BITBLIT $01,DST+14       ; n=0
        DONE

        org     MASKBUF
        dc.w    $FF00
        org     SRC
        dc.w    $F0F0
        org     SRC2
        dc.w    $CCCC
