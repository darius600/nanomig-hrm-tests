; mintdc.s -- ch6-minterms.c8: the five minterms of p.175's worked
; example ("A and not C or B") correspond to BLTCON0 bit positions 6, 4,
; 7, 3, 2 -- LF = $DC. A=FF00, B=F0F0, C=CCCC.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        ifnd    WRONGLF
        move.w  #$0FDC,BLTCON0(a0)     ; USEA+USEB+USEC+USED, LF=$DC
        else
        move.w  #$0FDE,BLTCON0(a0)     ; mutation: LF=$DE
        endc
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,SRC2
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT
        DONE

        org     MASKBUF
        dc.w    $FF00
        org     SRC
        dc.w    $F0F0
        org     SRC2
        dc.w    $CCCC
