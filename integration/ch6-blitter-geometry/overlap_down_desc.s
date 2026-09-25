; overlap_down_desc.s -- A7a: descending mode moves the same overlapping
; one-row-down block correctly. Pointers point at each block's LAST word
; (6-descending-mode.c9, ch6-desc.c10); modulo and blit size are the same
; as the ascending run (ch6-desc.c11).
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
        SETPTR  BLTAPTH,BLTAPTL,SRC+10          ; source's last word (row 2)
        SETPTR  BLTDPTH,BLTDPTL,SRC+14          ; destination's last word (row 3)
        BLTEN_ON
        move.w  #(3<<6)|2,BLTSIZE(a0)  ; height 3, width 2 words
        WAITBLT
        DONE

        org     SRC
        dc.w    $1111,$2222             ; row 0
        dc.w    $3333,$4444             ; row 1
        dc.w    $5555,$6666             ; row 2
        dc.w    $0000,$0000             ; row 3 (destination overflow)
