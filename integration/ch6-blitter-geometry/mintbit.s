; mintbit.s -- ch6-venn.c8: the inverse of a source, NOT A, is minterms
; 0, 1, 2 and 3 -- LF = $0F. A=FF00, B=F0F0, C=CCCC (B and C arbitrary).
        include "nanomig.i"
        include "geom.i"

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
        ifnd    WRONGBIT
        move.w  #$0F0F,BLTCON0(a0)     ; USEA+USEB+USEC+USED, LF=$0F (NOT A)
        else
        move.w  #$0F30,BLTCON0(a0)     ; mutation: LF=$30 (minterms 5,4, not 0-3)
        endc
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,SRC2
        SETPTR  BLTDPTH,BLTDPTL,DST
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT
        DONE

        org     MASKBUF
        dc.w    $FF00
        org     SRC
        dc.w    $F0F0
        org     SRC2
        dc.w    $CCCC
