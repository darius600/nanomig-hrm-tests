; overlap_up_desc.s -- A7b mutation: use descending mode for the same
; one-row-up move. Descending now reads an already-overwritten row.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0002,BLTCON1(a0)     ; mutation: DESC=1
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,SRC+14          ; row 3, last word
        SETPTR  BLTDPTH,BLTDPTL,SRC+10          ; row 2, last word
        BLTEN_ON
        move.w  #(3<<6)|2,BLTSIZE(a0)
        WAITBLT
        DONE

        org     SRC
        dc.w    $0000,$0000
        dc.w    $AAAA,$BBBB
        dc.w    $CCCC,$DDDD
        dc.w    $EEEE,$FFFF
