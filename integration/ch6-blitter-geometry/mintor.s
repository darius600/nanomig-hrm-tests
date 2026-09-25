; mintor.s -- A10: minterms combine by OR-ing their LF bits (ch6-venn.c11);
; "A and B" (regardless of C) is minterms 7 and 6, LF = $C0 (ch6-venn.c5).
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        ifnd    MISSINGBIT
        move.w  #$0FC0,BLTCON0(a0)     ; USEA+USEB+USEC+USED, LF=$C0 (A AND B)
        else
        move.w  #$0F80,BLTCON0(a0)     ; mutation: only minterm 7
        endc
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF        ; A
        SETPTR  BLTBPTH,BLTBPTL,SRC             ; B
        SETPTR  BLTCPTH,BLTCPTL,SRC2            ; C
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
