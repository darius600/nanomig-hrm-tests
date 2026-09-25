; overlap_up_asc.s -- A7b (control): if source and destination overlap,
; ascending mode is correct when the destination has the LOWER memory
; address (ch6-copy.c10) -- moving a block one row up. 4 rows x 2 words,
; in place: source = rows 1-3, destination = rows 0-2.
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
        SETPTR  BLTAPTH,BLTAPTL,SRC+4           ; row 1, first word
        SETPTR  BLTDPTH,BLTDPTL,SRC             ; row 0, first word
        BLTEN_ON
        move.w  #(3<<6)|2,BLTSIZE(a0)  ; height 3, width 2 words
        WAITBLT
        DONE

        org     SRC
        dc.w    $0000,$0000             ; row 0 (destination overflow)
        dc.w    $AAAA,$BBBB             ; row 1
        dc.w    $CCCC,$DDDD             ; row 2
        dc.w    $EEEE,$FFFF             ; row 3
