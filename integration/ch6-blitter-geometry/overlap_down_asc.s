; overlap_down_asc.s -- A7a (the "under suspicion" half): moving a block
; one row down (towards increasing addresses) in place with a standard
; (ascending) copy overwrites a later row before it is copied (ch6-desc.c1).
; 4 rows x 2 words, in place: source = rows 0-2, destination = rows 1-3.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)     ; ascending (DESC=0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,SRC             ; row 0, first word
        SETPTR  BLTDPTH,BLTDPTL,SRC+4           ; row 1, first word
        BLTEN_ON
        move.w  #(3<<6)|2,BLTSIZE(a0)  ; height 3, width 2 words
        WAITBLT
        DONE

        org     SRC
        dc.w    $1111,$2222             ; row 0
        dc.w    $3333,$4444             ; row 1
        dc.w    $5555,$6666             ; row 2
        dc.w    $0000,$0000             ; row 3 (destination overflow)
