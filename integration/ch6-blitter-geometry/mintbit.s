; mintbit.s -- A9: minterm n is bit n of the LF byte (ch6-venn.c2), checked
; at both ends: LF=$80 (minterm 7 alone) selects exactly where A, B and C
; are all 1; LF=$0F (minterms 0-3) selects exactly where A is 0, whatever
; B and C are (ch6-venn.c8). A, B, C source words cover all eight
; combinations across their 16 bit positions.
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

        ; blit 1: LF = $80, minterm 7 (A AND B AND C) alone. A/B/C are
        ; reloaded before each blit: once a blit completes, every active
        ; pointer has advanced past the end of its (1-word) buffer.
        ifnd    WRONGBIT
        move.w  #$0F80,BLTCON0(a0)
        else
        move.w  #$0F40,BLTCON0(a0)     ; mutation: minterm 6, not 7
        endc
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF        ; A
        SETPTR  BLTBPTH,BLTBPTL,SRC             ; B
        SETPTR  BLTCPTH,BLTCPTL,SRC2            ; C
        SETPTR  BLTDPTH,BLTDPTL,DST
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT

        ; blit 2: LF = $0F, minterms 0-3 (NOT A), A/B/C reloaded again
        move.w  #$0F0F,BLTCON0(a0)
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,SRC2
        SETPTR  BLTDPTH,BLTDPTL,DST2
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT
        DONE

        org     MASKBUF
        dc.w    $FF00
        org     SRC
        dc.w    $F0F0
        org     SRC2
        dc.w    $CCCC
