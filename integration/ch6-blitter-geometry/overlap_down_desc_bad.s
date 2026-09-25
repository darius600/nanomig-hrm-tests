; overlap_down_desc_bad.s -- A7a mutation: DESC=1 but the pointers are
; left at the block's first word instead of being moved to its last word.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0002,BLTCON1(a0)     ; DESC=1
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,SRC             ; mutation: still the first word
        SETPTR  BLTDPTH,BLTDPTL,SRC+4
        BLTEN_ON
        move.w  #(3<<6)|2,BLTSIZE(a0)
        WAITBLT
        DONE

        org     SRC
        dc.w    $1111,$2222
        dc.w    $3333,$4444
        dc.w    $5555,$6666
        dc.w    $0000,$0000
